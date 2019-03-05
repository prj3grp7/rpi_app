#include "I2C.h"

using namespace std;

int I2C::readI2C()
{
  int num_read = read(filePointer_, readBuffer_, 1);
  if (num_read == -1) {
    cout << "Read: Failed to open with err: " << strerror(errno) << endl << endl;
  }
  else if (num_read < sizeof(readBuffer_)) {
    cout << "Read less than requested, only " << num_read << " bytes" << endl << endl;
  }
  return num_read; //Number of bytes read
}

const char * I2C::getReadBuffer()
{
  	return readBuffer_;
}

int I2C::writeI2C(const char * command, int length)
{
	cout << "PlayersideWrite" << endl;
	int num_wr;
	for (int i = 0; i < length; i++) {
		writeBuffer_[i] = command[i];
	}
	num_wr = write(filePointer_, writeBuffer_, strlen(writeBuffer_));

	if (num_wr == -1) {
		cout << "Failed to open with err: " << strerror(errno) << endl << endl;
		return num_wr;
	}

	else if (num_wr < strlen(writeBuffer_)) {
		cout << "Did not write the whole string, only " << num_wr << " bytes" << endl << endl;
	}
	return 0;
}
