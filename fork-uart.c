/**
 * @file    fork-uart.c
 * 
 * @brief Uart Port test 

 * @author  Gabriel Caron
 * @date    2024-10-28
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>   // File Control Definitions
#include <termios.h> // POSIX Terminal Control Definitions
#include <unistd.h>  // UNIX Standard Definitions
#include <errno.h>   // ERROR Number Definitions

void vInitPortSerie(void);

// device port série à utiliser 
//const char *portTTY = "/dev/ttyGS0"; 
//const char *portTTY = "/dev/ttyS0";
const char *portTTY = "/dev/ttyS1";
//const char *portTTY = "/dev/ttyS2";
//const char *portTTY = "/dev/ttyS3";
//const char *portTTY = "/dev/ttyS4";
//const char *portTTY = "/dev/ttyS5";
//const char *portTTY = "/dev/ttyUSB0"; // ttyUSB0 is the FT232 based USB2SERIAL Converter

int fd; //File Descriptor

void main(void)
	{
	pid_t	pid;
	vInitPortSerie();

	pid = fork();
	if(pid == 0)
	{//enfant
    // Write data to serial port 
	char write_buffer[2];	// Buffer containing characters to write into port
	int  bytes_written  = 0;  	// Value for storing the number of bytes written to the port 
	while (write_buffer[0] != 'q')
	{
    write_buffer[0] = getchar();
	bytes_written = write(fd, write_buffer, 1); // use write() to send data to port 
										// "fd"                   - file descriptor pointing to the opened serial port
										//	"write_buffer"         - address of the buffer containing data
										// "sizeof(write_buffer)" - No of bytes to write 
	printf("\nTx: Ecriture de 1 octets : %c ecrit sur le port %s", (write_buffer[0] +65), portTTY);
	printf("\n");
	}

	}
	else
	{
	 //parent
	 // Read data from serial port 
	tcflush(fd, TCIFLUSH);  // Discards old data in the rx buffer
	char read_buffer[32];   // Buffer to store the data received 
	int  bytes_read = 0;    // Number of bytes read by the read() system call 
	int i = 0;
	while (read_buffer[0] != '!')
	{
	bytes_read = read(fd, &read_buffer, 32); // Read the data 
		
	printf("\nRx: Bytes Recus : %d --> ", bytes_read); // Print the number of bytes read
	for(i=0; i<bytes_read; i++)	 // printing only the received characters
		printf("%c", read_buffer[i]);
	printf("\n");
	}
	}

	}

	void vInitPortSerie(void)
	{
	printf("\n Init Port Serie\n");

	// Opening the Serial Port 
	fd = open(portTTY, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
								// O_RDWR Read/Write access to serial port           
								// O_NOCTTY - No terminal will control the process   
								// O_NDELAY -Non Blocking Mode,Does not care about-  
								// -the status of DCD line, Open() returns immediatly                                        
	if(fd == -1) // Error Checking
		printf("\n Erreur! ouverture de %s \n", portTTY);
	else
		printf("\n Ouverture de %s reussit \n", portTTY);

	// Setting the Attributes of the serial port using termios structure 
	struct termios SerialPortSettings;	// Create the structure 
	tcgetattr(fd, &SerialPortSettings);	// Get the current attributes of the Serial port
	// Setting the Baud rate
	cfsetispeed(&SerialPortSettings, B115200); // Set Read  Speed   
	cfsetospeed(&SerialPortSettings, B115200); // Set Write Speed  
	// 8N1 Mode 
	SerialPortSettings.c_cflag &= ~PARENB;   // Disables the Parity Enable bit(PARENB),So No Parity
	SerialPortSettings.c_cflag &= ~CSTOPB;   // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit
	SerialPortSettings.c_cflag &= ~CSIZE;	 // Clears the mask for setting the data size
	SerialPortSettings.c_cflag |=  CS8;      //Set the data bits = 8
	SerialPortSettings.c_cflag &= ~CRTSCTS;       // No Hardware flow Control
	SerialPortSettings.c_cflag |= CREAD | CLOCAL; // Enable receiver, Ignore Modem Control lines 

	SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);	// Disable XON/XOFF flow control both i/p and o/p

	SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Non Cannonical mode, Disable echo, Disable signal  

	SerialPortSettings.c_oflag &= ~OPOST;	//No Output Processing

	// Setting Time outs 
	SerialPortSettings.c_cc[VMIN] = 1; // Read at least 1 character(s) 
	SerialPortSettings.c_cc[VTIME] = 0; // Wait indefinetly 

	if((tcsetattr(fd, TCSANOW, &SerialPortSettings)) != 0) // Set the attributes to the termios structure
		printf("\n  Erreur! configuration des attributs du port serie");	
	}
