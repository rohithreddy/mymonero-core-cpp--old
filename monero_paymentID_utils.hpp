//
//  monero_paymentID_utils.hpp
//  MyMonero
//
//  Created by Paul Shapiro on 12/1/17.
//  Copyright (c) 2014-2018, MyMonero.com
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are
//  permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//	conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list
//	of conditions and the following disclaimer in the documentation and/or other
//	materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be
//	used to endorse or promote products derived from this software without specific
//	prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
//  THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
//  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//

#ifndef monero_paymentID_utils_hpp
#define monero_paymentID_utils_hpp

#include <stdio.h>
#include "crypto.h"

namespace monero_paymentID_utils
{
	//
	// Generating Payment IDs
	crypto::hash new_long_plain_paymentID(); // NOTE: This is deprecated – Try not to use it anymore
	crypto::hash8 new_short_plain_paymentID(); // This is favored - its length will be detected and encrypted automatically on send
	//
	// Parsing and Detecting Payment IDs
	bool parse_long_payment_id(const std::string& payment_id_str, crypto::hash& payment_id);
	bool parse_short_payment_id(const std::string& payment_id_str, crypto::hash8& payment_id);
	bool parse_payment_id(const std::string& payment_id_str, crypto::hash& payment_id);
}

#endif /* monero_paymentID_utils_hpp */
