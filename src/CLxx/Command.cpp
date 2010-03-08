
#include "Command.h"
#include "Host.h"
#include "Exception.h"
#include <algorithm>

namespace CLxx
{
	      Command::Command( bool blocking )
		  {
			  _isBlocking = blocking;
			  _waitingListDirty = false;
			  _enquedSuccessfully = false;
		  }

		  Command::~Command()
		  {
			  if ( _enquedSuccessfully )
		  		clReleaseEvent(_handle);
		  }


		  void Command::addToWaitingList( boost::shared_ptr<Command>& cmd )
		  {
			  if ( cmd.get() )
			  {
				  CommandWaitingList::iterator it = std::find(_waitingList.begin(), _waitingList.end(), cmd );
				  if ( it == _waitingList.end() )
				  {
					  _waitingList.push_back(cmd);
					  _waitingListDirty = true;
				  }
			  }
		  }
		  	
		  void Command::removeFromWaitingList( boost::shared_ptr<Command>& cmd )
		  {
			  if ( cmd.get() )
			  {
				  CommandWaitingList::iterator it = std::find(_waitingList.begin(), _waitingList.end(), cmd );
				  if ( it != _waitingList.end() )
				  {
					  _waitingList.remove(*it);		
					  _waitingListDirty = true;
				  }
			  }		      
		  }


		 void  Command::execute()
		 {
			_enquedSuccessfully = false;
		 	synchWaitingList();
			executePimp();
		 }

		 void Command::synchWaitingList()
		 {
		 	if ( _waitingListDirty )
			{
			   _waitingListDirty = false;
			   _waitingListHandles.clear();
			   for( CommandWaitingList::iterator it = _waitingList.begin(); it != _waitingList.end(); it++ )
			   {
				   _waitingListHandles.push_back((*it)->getHandle());
			   }
			}
		 }

		  Command::Status Command::getStatus() const
		  {
			if ( _cmdQueue.get() )
			{
				Status value;
				cl_int ciErrNum =  clGetEventInfo(_handle, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(Status), &value, NULL);

				if(ciErrNum != CL_SUCCESS)
					throw Exception(ciErrNum);
				
				return value;
			}
			else
			{
			   throw Exception("Commandhas not been enqued to any CommandQueue yet.");
			}
		  }
}
