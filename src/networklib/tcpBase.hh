#ifndef NETWORKLIB_HH
#define NETWORKLIB_HH

namespace networklib{

class tcpBase{
public:
	virtual ~tcpBase() = default;

	tcpBase(const tcpBase&) = delete;
	tcpBase(tcpBase&&) = delete;
	tcpBase &operator=(const tcpBase&) = delete;
	tcpBase &operator=(tcpBase&&) = delete;

	 

private:
	explicit tcpBase() = default;

protected:
	virtual int createTCPSocket() = 0;
	virtual int tcpRead() = 0;
	virtual int tcpWrite() = 0;

};


class udpBase{
public:
	virtual ~udpBase() = default;

	udpBase(const udpBase&) = delete;
	udpBase(udpBase&&) = delete;
	udpBase &operator=(const udpBase&) = delete;
	udpBase &operator=(udpBase&&) = delete;

private:
	explicit tcpBase() = default;

protected:
	virtual int createUDPSocket() = 0;
	virtual int udpRead() = 0;
	virtual int udpWrite() = 0;

};

} //networklib

#endif //NETWORKLIB_HH