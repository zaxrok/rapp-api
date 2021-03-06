#ifndef RAPP_CLOUD_FACEDETECTOR
#define RAPP_CLOUD_FACEDETECTOR
#include "includes.ihh"
namespace rapp {
namespace cloud {
/**
 * \class face_detection
 * \brief Asynchronous Service which will request the cloud to detect faces
 * \version 0.6.0
 * \date April 2016
 * \author Alex Gkiokas <a.gkiokas@ortelio.co.uk>
 */
class face_detection : public asio_service_http
{
public:
    /**
     * \brief Constructor
     * \param image is the input image \see rapp::object::picture
     * \param fast defines if this will be a fast call or not.
     * \param callback is the function that will receive a vector of detected face(s) 
     */
    face_detection(
                    const std::shared_ptr<rapp::object::picture> image,
                    bool fast,
					const std::string token,
                    std::function<void(std::vector<rapp::object::face>)> callback
                  )
    : asio_service_http(token), delegate_(callback)
    {
        assert(image);
        std::string boundary = random_boundary();
        std::string fname = random_boundary()+"."+image->type();
        boost::property_tree::ptree tree;
        tree.put("file", fname);
        tree.put("fast", boost::lexical_cast<std::string>(fast));
        std::stringstream ss;
        boost::property_tree::write_json(ss, tree, false);
		// set the `fast` param
        post_  = "--" + boundary + "\r\n"
               + "Content-Disposition: form-data; name=\"json\"\r\n\r\n"
               + ss.str() + "\r\n";
        // Create the Multi-form POST field 
        post_ += "--" + boundary + "\r\n"
              + "Content-Disposition: form-data; name=\"file_uri\"; filename\"" + fname + "\"\r\n"
              + "Content-Transfer-Encoding: binary\r\n\r\n";
        // Append binary data
        auto imagebytes = image->bytearray();
        post_.insert(post_.end(), imagebytes.begin(), imagebytes.end());
        post_ += "\r\n--" + boundary + "--";
        // Form the Header
        header_ = "POST /hop/face_detection HTTP/1.1\r\n";
                + "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n\r\n";
        // bind the base class callback, to our handle_reply
        callback_ = std::bind(&face_detection::handle_reply, this, std::placeholders::_1);
    }

private:
    /** 
	 * \brief handle the rapp-platform JSON reply
	 */
    void handle_reply(std::string json)
    {   
        std::stringstream ss(json);
        std::vector<rapp::object::face> faces;
        try {
            boost::property_tree::ptree tree;
            boost::property_tree::read_json(ss, tree);
            for (auto child : tree.get_child("faces")) {
                std::pair<float,float> up_left;
                std::pair<float,float> down_right;
                for (auto iter = child.second.begin(); iter!= child.second.end(); ++iter) {
                    if (iter->first == "up_left_point") {
                        for (auto it : iter->second) {
                            if (it.first == "x") {
                               std::get<0>(up_left) = it.second.get_value<float>();
							}
                            else if (it.first == "y") {
                               std::get<1>(up_left) = it.second.get_value<float>();
							}
                        }
                    }
                    else if (iter->first == "down_right_point") {
                        for (auto it : iter->second) {
                            if ( it.first == "x" ) {
                                std::get<0>(down_right) = it.second.get_value<float>();
							}
                            else if (it.first == "y") {
                                std::get<1>(down_right) = it.second.get_value<float>();
							}
                        }
                    }
                }
                faces.push_back(rapp::object::face(std::get<0>(up_left),
                                                   std::get<1>(up_left),
                                                   std::get<0>(down_right),
                                                   std::get<1>(down_right)));
            }
            // Check for Errors returned by the platform
            for (auto child : tree.get_child("error")) {
                const std::string value = child.second.get_value<std::string>();
                if (!value.empty()) {
                    std::cerr << "face_detection JSON error: " << value << std::endl;
                }
            }
        }
        catch (boost::property_tree::json_parser::json_parser_error & je) {
            std::cerr << "face_detection::handle_reply Error parsing: " << je.filename() 
                      << " on line: " << je.line() << std::endl;
            std::cerr << je.message() << std::endl;
        }
        delegate_(faces);
    }    
     
    /// The callback called upon completion of receiving the detected faces
    std::function<void(std::vector<rapp::object::face>)> delegate_;
};
}
}
#endif
