
#include "data_queues.hh"

  






single_thread_queue<ofq_item,DATA_FILE_QUEUE_LEN> _open_file_queue;

fan_out_thread_queue<eq_item,UNPACK_QUEUE_LEN>    _unpack_event_queue;

fan_in_thread_queue<eq_item,RETIRE_QUEUE_LEN>     _retire_queue;


void processor_thread_data_queues::init(int index)
{
  _unpack = &_unpack_event_queue._queues[index];
  _retire =       &_retire_queue._queues[index];
}


#ifndef NDEBUG
void debug_queues_status()
{
  TDBG_LINE("Open file queue:");
  _open_file_queue.debug_status();
  TDBG_LINE("Unpack event queue:");
  _unpack_event_queue.debug_status();
  TDBG_LINE("Retire event queue:");
  _retire_queue.debug_status();
}
#endif
