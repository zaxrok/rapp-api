#ifndef RAPP_CLOUD_TEXT2SPEECH
#define RAPP_CLOUD_TEXT2SPEECH
#include "includes.ihh"
namespace rapp {
namespace cloud {
/**
 * \class text_to_speech
 * \brief request speech audio from text
 * \version 0.6.0
 * \date July 2016
 * \author Alex Gkiokas <a.gkiokas@ortelio.co.uk>
 */
class text_to_speech : public asio_http
{
public:
    typedef rapp::object::microphone_wav wav_file;
	/**
	 * \brief construct handler which will request speech audio from text
	 * \param text is the text to be converted to audio
	 * \param language is language
	 * \callback will receive the speech audio object
	 */
	text_to_speech(
					 const std::string text,
					 const std::string language,
					 std::function<void(std::unique_ptr<rapp::object::microphone_wav>)> callback
				  )
	: asio_http(), delegate_(callback)
	{
        boost::property_tree::ptree tree;
        tree.put("text", text);
        tree.put("language", language);

        std::stringstream ss;
        boost::property_tree::write_json(ss, tree, false);

		std::string boundary = rapp::misc::random_boundary();
        post_  = "--" + boundary + "\r\n"
               + "Content-Disposition: form-data; name=\"json\"\r\n\r\n"
			   + ss.str();

		// set the HTTP header URI pramble and the Content-Type
        head_preamble_.uri = "POST /hop/text_to_speech HTTP/1.1\r\n";
		head_preamble_.content_type = "Content-Type: multipart/form-data; boundary=" + boundary;

        callback_ = std::bind(&text_to_speech::handle_reply, this, std::placeholders::_1);
	}

private:
    /**
     * \brief handle platform's JSON reply
     */
    void handle_reply(std::string json)
    {
        std::stringstream ss(json);
        std::vector<rapp::types::byte> bytearray;
        try {
            boost::property_tree::ptree tree;
            boost::property_tree::read_json(ss, tree);

		    // capture audio payload (base64-encoded)	
            for (auto child : tree.get_child("payload")) {
                // base64-encoded audio
                std::string result = child.second.get_value<std::string>();
                std::string decoded = rapp::misc::decode64(result);
                std::copy(decoded.begin(), decoded.end(), std::back_inserter(bytearray));
            }

			// get platform errors
            for (auto child : tree.get_child("error")) {
                const std::string value = child.second.get_value<std::string>();
                if (!value.empty()) {
                    std::cerr << "text_to_speech JSON error: " << value << std::endl;
                }
            }
	    }
        catch (boost::property_tree::json_parser::json_parser_error & je) {
            std::cerr << "text_to_speech::handle_reply Error parsing: " << je.filename() 
                      << " on line: " << je.line() << std::endl;
            std::cerr << je.message() << std::endl;
        }
        // create wav file and move it to the delegate
        auto wav = std::make_unique<rapp::object::microphone_wav>(bytearray);
        delegate_(std::move(wav));
    }

    /// 
    std::function<void(std::unique_ptr<wav_file> wav)> delegate_;
};
}
}
#endif