#pragma once
class nRFSN
{
public:
	nRFSN(void);
	~nRFSN(void);
	void init(char SPIFreq, char CSNpin, char IRQpin, char intNum);
	void setPower(char pwrLvl);
	void setTXMode(void);
	void setRXMode(void);
	void setMAX_RT(char numRT);
	void setChannel(unsigned char ch);
	unsigned char transmit(void);
	unsigned char getPayloadSize(void);
	int getPayload(void);
};

