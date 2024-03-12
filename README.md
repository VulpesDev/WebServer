## TEAM S0m3thing_With_5pid3r5
## webserv
42 Berlin core curriculum team project  
Simple, light-weight, non-blocking HTTP server  
### Requirements
Linux kernel 2.6.18 or newer  
### Installation
clone and make  
### Configuration
like nginx, but less  




Function = process_request
		else {
			std::cerr << "CGI HANDLING" << std::endl; //debug
			// return (response.send_cgi_response(cgi, req));
			// std::string cgi_result = 
            response.send_cgi_response(cgi, req);
            return response.getRawResponse();

		} 



Function = send_cgi_response
 else {
		HTTPResponse resp(200);
		if (request.method == "GET") {
			std::cout << "CGI get response" << std::endl;
			// resp.handle_cgi_get_response(resp, cgi_ret);
			handle_cgi_get_response(resp, cgi_ret);
		}
		else if (request.method == "POST") {
			handle_cgi_post_response(resp, cgi_ret);
		}
		std::string result = resp.getRawResponse();
		return result;
	}
	return "";

This should be checked for writing the response back to client
