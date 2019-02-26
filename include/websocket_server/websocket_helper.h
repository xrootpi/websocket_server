#pragma once
#include <string>
#include <boost/thread.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

class websocket_helper
{
public:
	websocket_helper();
	virtual ~websocket_helper();
	static int parse_masked_data(unsigned char* key, const unsigned char* data, unsigned char* actual_data);
	static void un_mask_data(const unsigned char* key, const unsigned char* data, const int len, unsigned char* un_masked_data);
	static void websocket_framing(std::string m_tx_buffer, unsigned char* buffer_to_send);
	static unsigned char* create_handshake_message(unsigned char* data);
private:
	static unsigned char* set_challenge_response(unsigned char * data);
	static unsigned char* get_header_value(unsigned char *data, const std::string& header);
	static unsigned char* string_to_char_array(const std::string& data);
	static std::string encode64(unsigned char* val);
};

