#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>

int main()
{
    std::string body =
    "------WebKitFormBoundarypIKW5zDegYBqDSmB\r\n"
    "Content-Disposition: form-data; name=\"example1\"; filename=\"simplefile1.txt\"\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "Hello world 1\r\n"
    "------WebKitFormBoundarypIKW5zDegYBqDSmB\r\n"
    "Content-Disposition: form-data; name=\"example2\"; filename=\"simplefile2.txt\"\r\n"
    "Content-Type: text/plain\r\n"
    "\r\n"
    "Hello world 2\r\n"
    "------WebKitFormBoundarypIKW5zDegYBqDSmB\r\n"
    "Content-Disposition: form-data; name=\"exampletext\"\r\n"
    "\r\n"
    "form field\r\n"
    "------WebKitFormBoundarypIKW5zDegYBqDSmB--\r\n";

	std::cout << "Original body:\n" << body << "\n\n";
	
	std::string contentType = "multipart/form-data; "
	                          "boundary=----WebKitFormBoundarypIKW5zDegYBqDSmB";
	std::string boundary =
	    "--" + contentType.substr(contentType.find("boundary=") + 9);
	std::string uploadPath = "./uploads";

	size_t pos = 0;
	size_t endPos;

	while ((pos = body.find(boundary, pos)) != std::string::npos)
	{
		pos += boundary.size();

		// Check for the end of the body
		if (body.substr(pos, 2) == "--")
		{
			break;
		}

		pos += 2; // To skip over the initial \r\n

		// Find the end of the part
		endPos = body.find(
		    boundary,
		    pos); // No need to subtract 2 because we now include \r\n in the part
		if (endPos == std::string::npos)
		{
			// This is the last part, the end of the part is the end of the body
			endPos = body.size();
		}

		// Extract the part
		std::string part = body.substr(pos, endPos - pos);

		// Check if the part is a file upload
		if (part.find("filename=\"") != std::string::npos)
		{
			// Extract the file name
			size_t      nameStart = part.find("filename=\"") + 10;
			size_t      nameEnd = part.find("\"", nameStart);
			std::string fileName = part.substr(nameStart, nameEnd - nameStart);

			// Extract the file content
			size_t fileStart = part.find("\r\n\r\n") + 4;
			size_t fileEnd = part.rfind("\r\n"); // Find the last "\r\n", which marks
			                                     // the end of the file content
			std::string fileContent = part.substr(fileStart, fileEnd - fileStart);

			// Save the file content to a file
			std::string   filePath = uploadPath + "/" + fileName;
			std::ofstream file(filePath, std::ios::binary);
			file << fileContent;
			file.close();

			// Replace the file upload part with a reference to the uploaded file
			std::string fileReference = "Content-Disposition: form-data; name=\"" +
			                            fileName + "\"\r\n\r\n" + filePath + "\r\n";
			body.replace(pos, endPos - pos, fileReference);
			pos += fileReference.size(); // Move pos to the end of the replaced part
		}
		else
		{
			pos = endPos;
		}
	}

	// Print the modified body
	std::cout << "Modified body:\n";
	std::cout << body << std::endl;

	return 0;
}
