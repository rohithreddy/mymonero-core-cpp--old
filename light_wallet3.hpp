//
//  light_wallet3.hpp
//  MyMonero
//
//  Created by Paul Shapiro on 1/17/18.
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
#pragma once

#include "wallet3_base.hpp"
#include "wallet2_transfer_utils.h" // for types
#include "monero_transfer_utils.hpp"
#include "light_wallet3_server_api.h"
//
#include "tools__ret_vals.hpp"
using namespace tools;
//
namespace tools
{
	// Sends view key to light wallet server (which connects to a daemon) for scanning
	class light_wallet3 : public wallet3_base
	{
	public:
		struct address_tx: tools::wallet2::payment_details // TODO: port wallet2 types to updated domain
		{
			bool m_coinbase;
			bool m_mempool;
			bool m_incoming;
			uint32_t m_mixin;
			std::string m_payment_id_string; // may be .empty()
		};
		struct get_rand_outs_promise_ret_vals: RetVals_base
		{
			boost::optional<std::string> response_json_string;
		};
		//
		//
		light_wallet3(bool testnet = false, bool restricted = false);
		//
		// Imperatives - Response reception
		void ingest__get_address_info(bool did_error, const light_wallet3_server_api::COMMAND_RPC_GET_ADDRESS_INFO::response &res); // sets connected
		// the following 'ingest__' methods assume successful responses
		void ingest__get_address_txs(const light_wallet3_server_api::COMMAND_RPC_GET_ADDRESS_TXS::response &ires);
		void ingest__get_unspent_outs(const light_wallet3_server_api::COMMAND_RPC_GET_UNSPENT_OUTS::response &ores, size_t light_wallet_requested_outputs_count);
		bool populate_from__get_random_outs(const light_wallet3_server_api::COMMAND_RPC_GET_RANDOM_OUTS::response &ores, std::vector<std::vector<tools::wallet2::get_outs_entry>> &outs, const std::vector<size_t> &selected_transfers, size_t fake_outputs_count, size_t requested_outputs_count, tools::RetVals_base &retVals) const;  // this would have been called ingest__ but it modifies no instance state
		//
		// Accessors - Overrides
		uint64_t balance(uint32_t index_major) const; 
		uint64_t unlocked_balance(uint32_t index_major) const;
		uint64_t blockchain_height() const;
		//
		// Accessors - Lightwallet hosted values
		uint64_t scanned_height() { return m_light_wallet_scanned_height; }
		uint64_t scanned_block_height() { return m_light_wallet_scanned_block_height; }
		uint64_t scan_start_height() { return m_light_wallet_scan_start_height; }
		uint64_t transaction_height() { return m_light_wallet_transaction_height; } 
		uint64_t total_received() { return m_light_wallet_total_received; }
		uint64_t locked_balance() { return m_light_wallet_locked_balance; } // aka "locked_funds"
		uint64_t total_sent() { return m_light_wallet_total_sent; }
		const std::unordered_map<crypto::hash, light_wallet3::address_tx> address_txs() { return m_light_wallet_address_txs; }
		//
		bool use_fork_rules(uint8_t version, int64_t early_blocks) const override; // required
		//
		// Transferring
		uint64_t get_dynamic_per_kb_fee_estimate() const override; // required; but this impl intentionally causes exception on call
		uint64_t get_per_kb_fee() const override;
		//
		bool create_signed_transaction( // assumes is_trusted_daemon=true
			const std::string &to_address_string,
			const std::string &amount_float_string,
			const std::string *optl__payment_id_string,
			uint32_t simple_priority,
			monero_transfer_utils::get_random_outs_fn_type get_random_outs_fn, // this function MUST be synchronous
			//
			wallet3_base::CreateTx_RetVals &retVals
		) const;
		void populate_amount_strings_for_get_random_outs(
			const std::vector<size_t> &selected_transfers, // select from
			std::vector<std::string> &amounts // to fill
		) const;
		uint32_t requested_outputs_count(size_t fake_outputs_count) const;
		//
	protected:
		bool m_light_wallet_connected;
		//
		uint64_t m_light_wallet_scanned_height;
		uint64_t m_light_wallet_scan_start_height;
		uint64_t m_light_wallet_transaction_height;
		uint64_t m_light_wallet_scanned_block_height;
		uint64_t m_light_wallet_blockchain_height;
		//
		uint64_t m_light_wallet_per_kb_fee = FEE_PER_KB;
		//
		uint64_t m_light_wallet_locked_balance; // aka "locked_funds"
		uint64_t m_light_wallet_total_received;
		uint64_t m_light_wallet_total_sent;
		//
		// Lightweight wallet info needed to populate m_payment requires 2 separate api calls (get_address_txs and get_unspent_outs)
		// We save the info from the first call in m_light_wallet_address_txs for easier lookup.
		std::unordered_map<crypto::hash, light_wallet3::address_tx> m_light_wallet_address_txs;
		// store calculated key image for faster lookup
		std::unordered_map<crypto::public_key, std::map<uint64_t, crypto::key_image> > m_key_image_cache;
		//
		bool is_own_key_image(const crypto::key_image& key_image, const crypto::public_key& tx_public_key, uint64_t out_index); // not declared as const b/c it mutates m_key_image_cache
		bool parse_rct_str(const std::string& rct_string, const crypto::public_key& tx_pub_key, uint64_t internal_output_index, rct::key& decrypted_mask, rct::key& rct_commit, bool decrypt) const;
		//
	};
}
BOOST_CLASS_VERSION(tools::light_wallet3, 1)



