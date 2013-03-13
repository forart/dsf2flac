/**
 * dsf2flac - http://code.google.com/p/dsf2flac/
 * 
 * A file conversion tool for translating dsf dsd audio files into
 * flac pcm audio files.
 * 
 *
 * Copyright (c) 2013 by respective authors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 
 * Acknowledgements
 * 
 * Many thanks to the following authors and projects whose work has greatly
 * helped the development of this tool.
 * 
 * 
 * Sebastian Gesemann - dsd2pcm (http://code.google.com/p/dsd2pcm/)
 * SACD Ripper (http://code.google.com/p/sacd-ripper/)
 * Maxim V.Anisiutkin - foo_input_sacd (http://sourceforge.net/projects/sacddecoder/files/)
 * Vladislav Goncharov - foo_input_sacd_hq (http://vladgsound.wordpress.com)
 * Jesus R - www.sonore.us
 * 
 */
 
 /**
  * dsf_sample_reader.h
  * 
  * Header file for the class dsfSampleReader.
  * 
  * Abstract class defining anything which reads dsd samples from something.
  * If someone feels like it they could write an implementation of this class
  * and allow this convertor to work with other dsd sources.
  * 
  */

#ifndef DSDSAMPLEREADER_H
#define DSDSAMPLEREADER_H

#include "stdio.h"
#include <boost/circular_buffer.hpp>

static const unsigned int defaultBufferLength = 5000;

class dsdSampleReader
{
public:
	// constructor and destructor
	dsdSampleReader();
	virtual ~dsdSampleReader();
	// CHILD CLASSES SHOULD OVERRIDE THESE!
	virtual bool step() {return false;}; // move buffer forward by 1byte (8bits) of sample data.
	virtual void rewind() {}; // resets the dsd reader to the start of the dsd data. Implementors should call clearBuffer.
	virtual long long int getLength() {return 1;}; // length of file in samples
	virtual unsigned int getNumChannels() {return 2;}; // number of channels in the file
	virtual unsigned int getSamplingFreq() {return 44100;}; // the sampling freq of the dsd data
	// CHILD CLASSES CAN OPTIONALLY OVERRIDE THESE!
	virtual char* getArtist() {return NULL;} 
	virtual char* getAlbum() {return NULL;}
	virtual char* getTitle() {return NULL;}
	virtual char* getTrack() {return NULL;}
	virtual char* getYear() {return NULL;}
	virtual bool msbIsYoungest() {return true;} // the data is stored in 8-bit chars, returns true if the left most bit (msb) is the youngest.
	virtual unsigned char getIdleSample() { return 0x69; }; // returns the idle tone used by this reader.
	virtual bool samplesAvailable() { return getPosition()<getLength(); }; // false when no more samples left
	// positioning.
	// public methods
	boost::circular_buffer<unsigned char>* getBuffer(); // get the char sample buffers (1 per channel) by default filled with getIdleSample()
	bool setBufferLength(unsigned int bufferLength); // you can use this to set the length of the buffer WARNING: causes the file to be rewound!
	unsigned int getBufferLength(); // return the length of the circular buffers
	long long int getPosition() {return posMarker*samplesPerChar;}; // current position in the file in dsd samples
	long long int getPosition(long long int bufferPos) { return getPosition() - bufferPos; }; // get the position in dsd samples in relation to a certain position in the buffer.
	double getPositionInSeconds(); // current position in the file in seconds
	double getPositionAsPercent();
	double getLengthInSeconds(); // length of file in seconds
	bool isValid(); // returns false if the reader is invalid
	std::string getErrorMsg();
	// static method to help out with latin1 charset
	static char* latin1_to_utf8(char* latin1);
	static unsigned char* latin1_to_utf8(unsigned char* latin1);
protected:
	// protected properties
	boost::circular_buffer<unsigned char>* circularBuffers;
	// position marker
	long long int posMarker; // implementors need to increment this on step()
	unsigned int samplesPerChar; // should be set by implementors
	// to hold feedback on errors
	bool valid;
	std::string errorMsg;
protected:
	// protected methods
	void allocateBuffer(); //implementors need to call this once they know the number of channels!
	void clearBuffer(); // fill entire buffer with idleSample
	void resizeBuffer(); // resize buffer, fill with idleSample, call rewind
private:
	// private properties
	unsigned int bufferLength;
	bool isBufferAllocated;
};
#endif // DSDSAMPLEREADER_H
