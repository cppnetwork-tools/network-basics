#ifndef NETWORKLIB_HH
#define NETWORKLIB_HH

namespace networklib{

class tcpBase{
public:
	virtual ~tcpwrapper() = default;

	tcpWrapper(const tcpwrapper&) = delete;
	tcpwrapper(tcpwrapper&&) = delete;
	tcpwrapper &operator=(const tcpwrapper&) = delete;
	tcpwrapper &operator=(tcpwrapper&&) = delete;

	 

private:
	explicit tcpWrapper() = default;

protected:
	int createTcpSocket() = 0;
	int readFrom() = 0;
	int writeFrom() = 0;

 
};

} //networklib

#endif //NETWORKLIB_HH