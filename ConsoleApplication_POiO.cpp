#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <array>
#include "ffft/FFTReal.h"
using namespace std;


class Packet
{
protected:
	string device;
	string description;
	long date;
public:
	Packet(string dev = "urzadzenie", string des = "opis", long dat = 10101998)
	{
		device = dev;
		description = des;
		date = dat;
	}
	virtual void toString() = 0;
};

template <class T, int i>
class Sequence :public Packet
{
protected:
	int channelNr;
	string unit;
	double resolution;
	T buffer[i];
public:
	Sequence(T buf, string dev = "urzadzenie", string des = "opis", long dat = 10101998, int chNr = 1, string unt = "jednostka", double res = 100.5)
		:Packet(dev, des, dat)
	{
		channelNr = chNr;
		unit = unt;
		resolution = res;
		for (int n = 0; n < i; n++)
		{
			buffer[n] = n;
		}
	}
	virtual void toString() = 0;
};

template <class T, int i>
class TimeHistory :public Sequence <char, 3>
{
	double sensitivity;
public:
	TimeHistory(string dev = "urzadzenie", string des = "opis", long dat = 10101998, int chNr = 1, string unt = "jednostka", double res = 100, T buf = 1, double sens = 15.5)
		:Sequence(buf, dev, des, dat, chNr, unt, res)
	{
		sensitivity = sens;
	}
	virtual void toString()
	{
		cout << "buffer: ";
		for (int n = 0; n < i; n++)
		{
			cout << buffer[n];
		}
		cout << " ";
		cout << "device: " << device << ", description: " << description << ", date: " << to_string(date) << ", channelNr: " << to_string(channelNr) << ", unit: " << unit << ", resolution: " << to_string(resolution) << ", sensitivity: " << to_string(sensitivity);
	}
};

class Spectrum :public Packet
{
	double scaling;
public:
	Spectrum(string dev = "urzadzenie", string des = "opis", long dat = 10101998, double scal = 20.15)
		:Packet(dev, des, dat)
	{
		scaling = scal;
	}
	virtual void toString()
	{
		cout << "device: " << device << ", description: " << description << ", date: " << to_string(date) << ", scaling: " << to_string(scaling) << endl;
	}
};



class Alarm :public Packet
{
	int channelNr;
	float threshold;
	int direction;

public:

	Alarm(string dev = "urzadzenie", string des = "opis", long dat = 10101998, int chNr = 5, float thrshld = 10.5, int dir = 1)
		:Packet(dev, des, dat)
	{
		channelNr = chNr;
		threshold = thrshld;
		direction = dir;
	}
	virtual void toString()
	{
		cout << "device: " << device << ", description: " << description << ", date: " << to_string(date) << ", channelNr: " << to_string(channelNr) << ", threshold: " << to_string(threshold) << ", direction: " << direction << endl;
	}

};

// ----- LAB_2 ----- //

template <class T, int i>
class SignalGenerator
{
public:
	T signal[i];
	T fft[i];
	T abs_fft[i];
	float rms;

	SignalGenerator()
	{}

	SignalGenerator(T signal[], T fft[], T abs_fft[], float rms)
	{
		for (int n = 0; n < i; n++)
		{
			this->signal[n] = signal[n];
			this->fft[n] = fft[n];
			this->abs_fft[n] = abs_fft[n];
		}
		this->rms = rms;
	}

	SignalGenerator operator + (SignalGenerator& v)
	{
		SignalGenerator output;
		try
		{
			for (int n = 0; n < 3999; n++)
			{
				output.fft[n] = fft[n] + v.fft[n];
				output.signal[n] = signal[n] + v.signal[n];
				output.abs_fft[n] = abs_fft[n] + v.abs_fft[n];
			}

			output.rms = rms + v.rms;
		}
		catch (string error)
		{
			cout << error << endl;
		}
		return output;
	}

	SignalGenerator operator / (SignalGenerator& v)
	{
		SignalGenerator output;
		try
		{
			for (int n = 0; n < 3999; n++)
			{
				output.fft[n] = fft[n] / v.fft[n];
				output.signal[n] = signal[n] / v.signal[n];
				output.abs_fft[n] = abs_fft[n] / v.abs_fft[n];
			}

			output.rms = rms / v.rms;
		}
		catch (string error)
		{
			cout << error << endl;
		}
		return output;
	}

	void record(SignalGenerator s)
	{
		try
		{
			ofstream MyFile;
			MyFile.open("record.txt");
			MyFile << "amplitude:           fft:            abs(fft): \n";

			for (int n = 0; n < 3999; n++)
			{
				MyFile << signal[n] << "        " << fft[n] << "       " << abs_fft[n] << endl;
			}
			MyFile.close();
		}
		catch (string error)
		{
			cout << error << endl;
		}
	}

};

float gauss(float mean, float variation)
{
	float val_min, val_max;
	try
	{
		val_min = (mean - variation);
		val_max = (mean + variation);
	}
	catch (string error)
	{
		cout << error << endl;
	}
	float val = (float)rand() / RAND_MAX;
	return val_min + val * (val_max - val_min);
}


float RootMeanSquare(float amplitude[])
{
	float a = 0;
	float b = 0;
	float c = 0;
	float RMS = 0;

	try
	{
		for (int n = 0; n < 3999; n++)
		{
			c = c + amplitude[n];
		}

		a = c / 3999;

		for (int n = 0; n < 3999; n++)
		{
			b += pow((amplitude[n] - a), 2);
		}

		RMS = sqrt((1 / (float)3998) * b);
	}
	catch (string error)
	{
		cout << error << endl;
	}
	return RMS;
}

int main()
{

	float S1[4096];
	float S2[4096];
	float S3[4096];
	float S4[4096];

	float f1[4096];
	float f2[4096];
	float f3[4096];
	float f4[4096];

	float fa1[4096];
	float fa2[4096];
	float fa3[4096];
	float fa4[4096];

	float r1;
	float r2;
	float r3;
	float r4;

	float len = 4096;

	ffft::FFTReal <float> fft_object(len);

	for (int n = 0; n < 3999; n++)
	{
		S1[n] = gauss(3, 0.3);
		f1[n] = 0;
		fa1[n] = 0;
		S2[n] = gauss(6, 0.3);
		f2[n] = 0;
		fa2[n] = 0;
		S3[n] = gauss(8, 0.1);
		f3[n] = 0;
		fa3[n] = 0;
		S4[n] = gauss(3, 0.3);
		f4[n] = 0;
		fa4[n] = 0;
	}

	r1 = RootMeanSquare(S1);
	r2 = RootMeanSquare(S2);
	r3 = RootMeanSquare(S3);
	r4 = RootMeanSquare(S4);

	fft_object.do_fft(f1, S1);
	fft_object.do_fft(f2, S2);
	fft_object.do_fft(f3, S3);
	fft_object.do_fft(f4, S4);

	for (int n = 0; n < 3999; n++)
	{
		fa1[n] = abs(f1[n]);
		fa2[n] = abs(f2[n]);
		fa3[n] = abs(f3[n]);
		fa4[n] = abs(f4[n]);
	}

	SignalGenerator<float, 3999> s1(S1, f1, fa1, r1);
	SignalGenerator<float, 3999> s2(S2, f2, fa2, r2);
	SignalGenerator<float, 3999> s3(S3, f3, fa3, r3);
	SignalGenerator<float, 3999> s4(S4, f4, fa4, r4);
	SignalGenerator<float, 3999> s2p3;
	SignalGenerator<float, 3999> s1b4;

	s2p3 = s2 + s3;
	s1b4 = s1 / s4;

	remove("record.txt");

	s1.record(s1);
	s2.record(s2);
	s3.record(s3);
	s4.record(s4);
	s2p3.record(s2p3);
	s1b4.record(s1b4);

	cout << "RMS channel 1: " << s1.rms << endl;
	cout << "RMS channel 2: " << s2.rms << endl;
	cout << "RMS channel 3: " << s3.rms << endl;
	cout << "RMS channel 4: " << s4.rms << endl;
	cout << "RMS channel 2 + channel 3 = " << s2p3.rms << endl;
	cout << "RMS channel 1 / channel 4 = " << s1b4.rms << endl;

	return 0;
}
