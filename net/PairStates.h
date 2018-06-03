#ifndef HAP_NET_PAIR_STATES
#define HAP_NET_PAIR_STATES


namespace hap {

namespace net {

	typedef enum
	{
		State_M1_SRPStartRequest = 1,
		State_M2_SRPStartRespond = 2,
		State_M3_SRPVerifyRequest = 3,
		State_M4_SRPVerifyRespond = 4,
		State_M5_ExchangeRequest = 5,
		State_M6_ExchangeRespond = 6,
	} PairSetupState;

	typedef enum
	{
		State_Pair_Verify_M1 = 1,
		State_Pair_Verify_M2 = 2,
		State_Pair_Verify_M3 = 3,
		State_Pair_Verify_M4 = 4,
	} PairVerifyState;

}

}

#endif // !HAP_NET_PAIR_SETUP_STATE
