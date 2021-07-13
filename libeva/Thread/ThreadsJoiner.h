#ifndef _LIBEVA_THREAD_THREADSJOINER_H_
#define _LIBEVA_THREAD_THREADSJOINER_H_

namespace libeva {

template<typename Container>
class ThreadsJoiner {
public:
	explicit ThreadsJoiner( Container &container ) :
		container_( container ) { } 
	~ThreadsJoiner() {
		for ( auto &thread : container_ ) {
			if ( thread.joinable() ) {
				thread.join();
			}
		}
	}
private:
	Container &container_;
};

}


#endif /* _LIBEVA_THREAD_THREADSJOINER_H_ */
