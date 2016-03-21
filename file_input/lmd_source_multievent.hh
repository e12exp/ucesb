#ifndef _LMD_SOURCE_MULTIEVENT_H_
#define _LMD_SOURCE_MULTIEVENT_H_

#include <deque>
#include <list>

#include <stdint.h>

struct lmd_source_multievent;
struct lmd_event_multievent;

#include "lmd_input.hh"
#include "thread_buffer.hh"

#define _ENABLE_TRACE 0

#if _ENABLE_TRACE
#define _TRACE(...) printf(__VA_ARGS__)
#else
#define _TRACE(...)
#endif

struct keep_buffer_wrapper : public keep_buffer_many
{
protected:
   int refcount;

public:
   keep_buffer_wrapper() : refcount(0) {};

   void release()
   {
      _TRACE("[%p] keep_buffer_wrapper::release(). refcount = %d\n", this, refcount);
      if(!refcount)
         return;

      if(--refcount == 0)
      {
         _TRACE("[%p] Release\n", this);
         keep_buffer_many::release();
      }

      assert(refcount >= 0);
   }

   void* allocate(size_t size)
   {
      refcount++;
      _TRACE("[%p] keep_buffer_wrapper::allocate(). refcount = %d\n", this, refcount);
      return keep_buffer_many::allocate(size);
   }

   bool available()
   {
      _TRACE("[%p] keep_buffer_wrapper::available() => %d\n", this, !refcount);
      return !refcount;
   }
};

struct multievent_entry
{
        keep_buffer_wrapper *data_alloc;

	lmd_subevent_10_1_host _header;
	
	uint64_t timestamp;
	uint32_t channel;
	
	uint32_t *data;
	uint32_t size;
	
	multievent_entry(keep_buffer_wrapper *data_alloc) : data_alloc(data_alloc), data(NULL) {}
	~multievent_entry()
	{
	       _TRACE("multievent_entry::dtor()\n");

               if(data != NULL)
               {
                  data_alloc->release();
                  data = NULL;
               }
	}

        static bool compare(const multievent_entry *e1, const multievent_entry *e2);

        void* operator new(size_t bytes, keep_buffer_wrapper &alloc);
        void operator delete(void *ptr);
};

typedef std::deque< multievent_entry* > multievent_queue;

struct lmd_source_multievent : public lmd_source
{
protected:
        enum file_status_t { ready, eof, unknown_event };

        std::list<keep_buffer_wrapper*> data_alloc;
        std::list<keep_buffer_wrapper*>::iterator curbuf;

        file_status_t input_status;

	lmd_event_hint event_hint;
	lmd_event_10_1_host input_event_header;
	sint32 l_count;
	
	multievent_queue events_available;
	multievent_queue events_curevent;
        multievent_queue events_read;

	file_status_t load_events();
	
	multievent_entry* next_singleevent();
public:

	lmd_source_multievent(); 

   virtual lmd_event *get_event();
};

#endif

