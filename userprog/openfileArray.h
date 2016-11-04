#ifndef OPENFILE_ARRAY_H
#define OPENFILE_ARRAY_H

#include "synch.h"
#include "syscall.h"

#define MaxOpenFiles 128

class OpenFileArray{
	public:
		OpenFileArray();

	private:
		OpenFile *openFiles[MaxOpenFiles];
};

#endif