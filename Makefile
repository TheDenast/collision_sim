# vks_project

vks_project: main.o DE1SoCfpga.o SevenSegment.o SoundPlayer.o VgaDrawer.o
	g++ main.o DE1SoCfpga.o SevenSegment.o SoundPlayer.o VgaDrawer.o -o vks_project 	

main.o: main.cpp DE1SoCfpga.h
	g++ -g -Wall -c main.cpp

DE1SoCfpga.o: DE1SoCfpga.cpp DE1SoCfpga.h
	g++ -g -Wall -c DE1SoCfpga.cpp

SevenSegment.o: SevenSegment.cpp SevenSegment.h
	g++ -g -Wall -c SevenSegment.cpp

SoundPlayer.o: SoundPlayer.cpp SoundPlayer.h
	g++ -g -Wall -c SoundPlayer.cpp

VgaDrawer.o: VgaDrawer.cpp VgaDrawer.h
	g++ -g -Wall -c VgaDrawer.cpp

clean:
	rm main.o DE1SoCfpga.o SevenSegment.o SoundPlayer.o VgaDrawer.o vks_project


