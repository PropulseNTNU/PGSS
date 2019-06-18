#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstdint>

#define NUMBER_OF_DATA_TYPES 9 // IMPORTANT!! Set correct number of datatypes that are sent/received

int findPackage(char* buffer, int bufferSize, int startIndex, uint8_t* output, uint16_t* packageNumber);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./parse_data <input_filename> <output_filename>" << std::endl;
        return -1;
    }

    // Find size of file
    int size;
    struct stat results;
    if (stat(argv[1], &results) == 0)
        size = results.st_size;
    else
        return -1;
    std::cout << "Size of file: " << size << std::endl;

    // Allocate buffer and data array
    char* buffer = new char[size];
    int maxNumPackages = size / (NUMBER_OF_DATA_TYPES*sizeof(float)+sizeof(uint16_t)+2);
    float* data = new float[maxNumPackages] {};

    // Open and read file into buffer
    std::ifstream inputFile(argv[1], std::ios::in | std::ios::binary); // Open with binary flag set
    inputFile.read(buffer, size);
    
    // Open output file
    std::ofstream outputFile;
    outputFile.open(argv[2]);

    int index = 0; // Startindex for next package search
    uint16_t packageNumber = 0;
    int numberOfPackages = 0;
    while (index < size) {
        index = findPackage(buffer, size, index, (uint8_t*)data, &packageNumber);
        for (int i = 0; i < NUMBER_OF_DATA_TYPES; i++)
            outputFile << data[i] << ","; // Write package to file
        outputFile << packageNumber << "\n";
        numberOfPackages++;
    }
    std::cout << "Found : " << numberOfPackages << " packages." << std::endl;
    outputFile.close();
   
    // Clean up
    delete[] buffer;
    delete[] data;
    buffer = nullptr;
    return 0;
}

int findPackage(char* buffer, int bufferSize, int startIndex, uint8_t* output, uint16_t* packageNumber) {
    int package_size = sizeof(*packageNumber) + NUMBER_OF_DATA_TYPES*sizeof(float); 
    int i = startIndex;
    uint8_t data;
    
    // Find start of package in buffer after *index*
	while (true) {
        data = buffer[i];
        if (i >= (bufferSize-package_size))
            return bufferSize;
        if ( (data == '<') &&  (buffer[i+package_size+1] == '>') && (buffer[i+package_size+2] == '<') ){
            break;
        }
        i++;
    }
    i++; // Break skips the last increment ;)) :((

	//Read package data
    for (uint8_t j = 0; j < package_size; ++j) {
        data = buffer[i + j];
        if (j < sizeof(*packageNumber))
            ((uint8_t*) packageNumber)[j] = data;
        else if (j < package_size)
			output[j - sizeof(*packageNumber)] = data;
    }
    return i+1;
}