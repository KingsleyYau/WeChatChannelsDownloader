/*
 * KRunnable.h
 *
 *  Created on: 2014/10/27
 *      Author: Max.Chiu
 *      Email: Kingsleyyau@gmail.com
 */

#ifndef KRUNNABLE_H_
#define KRUNNABLE_H_

class KRunnable {
public:
	virtual void onRun() = 0;
	virtual ~KRunnable(){};
};


#endif /* KRUNNABLE_H_ */
