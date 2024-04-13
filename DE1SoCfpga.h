#ifndef DE1SOCFPGA_H
#define DE1SOCFPGA_H
	
class DE1SoCfpga  
{
	public:
	char *pBase;
    int fd;
	DE1SoCfpga();
	~DE1SoCfpga();
	void RegisterWrite(char *pBase, unsigned int reg_offset, int value);
	int RegisterRead(char *pBase, unsigned int reg_offset);
};
#endif