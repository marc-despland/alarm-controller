#include "blueping.h"
#include "log.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>



int BluePing::ping(std::string target) {
   uint8_t svc_uuid_int[] = { 0, 0, 0x10, 0, 0, 0, 0x10, 0, 0x80, 0, 0, 0x80, 0x5f, 0x9b, 0x34, 0xfb};
    uuid_t svc_uuid;
    int err;
    bdaddr_t btarget;
    bdaddr_t my_bdaddr_any = { 0 };
    sdp_list_t *response_list = NULL, *search_list, *attrid_list;
    sdp_session_t *session = 0;

    str2ba( target.c_str(), &btarget );
    int ret=0;
    // connect to the SDP server running on the remote machine
    session = sdp_connect( &my_bdaddr_any, &btarget, SDP_RETRY_IF_BUSY );
    Log::logger->log("BLUEPING", DEBUG) << "sdp connect" <<endl;
    if (session ==NULL) {
    	Log::logger->log("BLUEPING", NOTICE) << "Can't connect to device" <<endl;
    	return 0;
    }
    // specify the UUID of the application we're searching for
    sdp_uuid128_create( &svc_uuid, &svc_uuid_int );
    search_list = sdp_list_append( NULL, &svc_uuid );

    // specify that we want a list of all the matching applications' attributes
    uint32_t range = 0x0000ffff;
    attrid_list = sdp_list_append( NULL, &range );

    // get a list of service records that have UUID 0xabcd
    err = sdp_service_search_attr_req( session, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &response_list);
    sdp_list_t *r = response_list;
    Log::logger->log("BLUEPING", DEBUG) << "Search Services" <<endl;
    // go through each of the service records
    for (; r; r = r->next ) {
        sdp_record_t *rec = (sdp_record_t*) r->data;
        sdp_list_t *proto_list;
        
        // get a list of the protocol sequences
        if( sdp_get_access_protos( rec, &proto_list ) == 0 ) {
        sdp_list_t *p = proto_list;

        // go through each protocol sequence
        for( ; p ; p = p->next ) {
            sdp_list_t *pds = (sdp_list_t*)p->data;

            // go through each protocol list of the protocol sequence
            for( ; pds ; pds = pds->next ) {

                // check the protocol attributes
                sdp_data_t *d = (sdp_data_t*)pds->data;
                int proto = 0;
                for( ; d; d = d->next ) {
                    switch( d->dtd ) { 
                        case SDP_UUID16:
                        case SDP_UUID32:
                        case SDP_UUID128:
                            proto = sdp_uuid_to_proto( &d->val.uuid );
                            break;
                        case SDP_UINT8:
                            if( proto == RFCOMM_UUID ) {
                                //printf("rfcomm channel: %d\n",d->val.int8);
                                Log::logger->log("BLUEPING", DEBUG) << "rfcomm channel: " << d->val.int8 << endl;
                            }
                            break;
                    }
                }
            }
            sdp_list_free( (sdp_list_t*)p->data, 0 );
        }
        sdp_list_free( proto_list, 0 );

        }
        ret=1;
        //printf("found service record 0x%x\n", rec->handle);
        Log::logger->log("BLUEING", DEBUG) <<  "found service record " <<rec->handle<<endl;
        sdp_record_free( rec );
    }
    Log::logger->log("BLUEPING", DEBUG) << "Closing session" <<endl;
    sdp_close(session);
    return ret;
}