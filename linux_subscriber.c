#include <unistd.h>
#include "linux_subscriber_sdds_impl.h"

int main()
{
    DDS_ReturnCode_t ret;

    if (sDDS_init() == SDDS_RT_FAIL) {
        return 1;
    }
    Log_setLvl(6);  // Disable logs, set to 0 for to see everything.

    static SingleInputContact singleinputcontact_sub;
    SingleInputContact *singleinputcontact_sub_p = &singleinputcontact_sub;
    static OnOffLight onofflight_sub;
    OnOffLight *onofflight_sub_p = &onofflight_sub;
    static SingleInputContactNonce singleinputcontactnonce_sub;
    SingleInputContactNonce *singleinputcontactnonce_sub_p = &singleinputcontactnonce_sub;

    for (;;) {
	    int last_nonce = 0;
        ret = DDS_SingleInputContactDataReader_take_next_sample(g_SingleInputContact_reader,
                &singleinputcontact_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for singleinputcontact\n");
        }
        else {
            printf("Received a sample from topic 'singleinputcontact': {\n"
                   "   id => %"PRIu16"\n"
                   "   contact => %"PRIu8"\n"
                   "   OutOfService => %d\n"
                   "}\n"
                   , singleinputcontact_sub_p->id
                   , singleinputcontact_sub_p->contact
                   , singleinputcontact_sub_p->outofservice);
        }

        ret = DDS_OnOffLightDataReader_take_next_sample(g_OnOffLight_reader,
                &onofflight_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for onofflight\n");
        }
        else {
            printf("Received a sample from topic 'onofflight': {\n"
                   "   id => %"PRIu16"\n"
                   "   state => %"PRIu8"\n"
                   "}\n"
                   , onofflight_sub_p->id
                   , onofflight_sub_p->state);
        }

	ret = DDS_SingleInputContactNonceDataReader_take_next_sample(g_SingleInputContactNonce_reader,
                &singleinputcontactnonce_sub_p, NULL);
        if (ret == DDS_RETCODE_NO_DATA) {
            printf("no data for singleinputcontactnonce\n");
        }
        else {
            int nonce = singleinputcontactnonce_sub_p->nonce;
	    time_t t;
	    bool correct = false;
	    t = time(NULL);
	    t = t/30;
	    t = t&0x0000FFFF;
	    printf("%x\n", t);
	    if(t == nonce || t == nonce-1 || t == nonce+1){
		    if( last_nonce != nonce){
		    	last_nonce = nonce;	
			correct = true;
		    }
	    }
		printf("Received a sample from topic 'singleinputcontactnonce': {\n"
                   "   id => %"PRIu16"\n"
                   "   contact => %"PRIu8"\n"
		   "   nonce => %"PRIu16"\n"
                   "   OutOfService => %d\n"
		   "   Correct => %s\n"
                   "}\n"
                   , singleinputcontactnonce_sub_p->id
                   , singleinputcontactnonce_sub_p->contact
		   , singleinputcontactnonce_sub_p->nonce
                   , singleinputcontactnonce_sub_p->outofservice
		   , correct ? "true" : "false" );
        }

        sleep (1);
    }

    return 0;
}
