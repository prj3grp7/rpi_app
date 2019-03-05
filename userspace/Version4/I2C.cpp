#include "I2C.h"
#include "string.h"

using namespace std;

int I2C::readI2C()
{
  int num_read = read(filePointer_, readBuffer_, 1);
  if (num_read == -1) {
    cout << "Read: Failed to open with err: " << strerror(errno) << endl << endl;
  }

  /*
  else if (num_read < sizeof(readBuffer_)) {
    cout << "Read less than requested, only " << num_read << " bytes" << endl << endl;
  }
  */

  return num_read; //Number of bytes read
}

const char * I2C::getReadBuffer()
{
  	return readBuffer_;
}

int I2C::writeI2C(const char * command, int length)
{
	int num_wr;
	for (int i = 0; i < length; i++) {
		writeBuffer_[i] = command[i];
	}
	num_wr = write(filePointer_, writeBuffer_, length);

	if (num_wr == -1) {
		cout << "Write: Failed to open with err: " << strerror(errno) << endl << endl;
    memset(writeBuffer_, 0, strlen(writeBuffer_));
		return num_wr;
	}

/*
	else if (num_wr < strlen(writeBuffer_)) {
		cout << "Did not write the whole string, only " << num_wr << " bytes" << endl << endl;
	}
*/
  memset(writeBuffer_, 0, strlen(writeBuffer_));
	return 0;
}
