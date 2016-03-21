#include "reclaim.hh"

#include "thread_buffer.hh"
#include "file_mmap.hh"
#include "pipe_buffer.hh"

#include "optimise.hh"

void reclaim_items(reclaim_item *first)
{
  // Note, after we have called the function to reclaim the item, we
  // may no longer reference the item, (e.g. the _next pointer), since
  // usually the reclaim info will be allocated together with the
  // object itself

  while (first)
    {
      uint32 type = first->_type;

      reclaim_item *item = first;
      first              = first->_next;

      MFENCE; // to assure the load is not delayed beyond the function
	      // calls below.  The switch can anyhow not proceed until
              // type has been loaded

      switch (type)
	{
	case RECLAIM_MESSAGE:
	  {
	    tb_reclaim *tbr = (tb_reclaim*) item;

	    reclaim_eject_message(tbr);
	  }
	  break;
	case RECLAIM_FORMAT_BUFFER_HEADER:
	case RECLAIM_THREAD_BUFFER_ITEM:
	  {
	    tb_reclaim *tbr = (tb_reclaim*) item;

	    tbr->_buffer->reclaim(tbr->_reclaim);
	  }
	  break;
	case RECLAIM_MMAP_RELEASE_TO:
	  {
	    fmm_reclaim *fmmr = (fmm_reclaim*) item;

	    fmmr->_mm->release_to(fmmr->_end);
	    fmmr->_buffer->reclaim(fmmr->_reclaim);
	  }
	  break;
	case RECLAIM_PBUF_RELEASE_TO:
	  {
	    pbf_reclaim *pbfr = (pbf_reclaim*) item;

	    pbfr->_pb->release_to(pbfr->_end);
	    pbfr->_buffer->reclaim(pbfr->_reclaim);
	  }
	  break;
	default:
	  ERROR("Reclaim internal error.");
	  break;
	}
    }
}

void print_format_reclaim_items(reclaim_item *first)
{
  // Note, after we have called the function to reclaim the item, we
  // may no longer reference the item, (e.g. the _next pointer), since
  // usually the reclaim info will be allocated together with the
  // object itself

  while (first)
    {
      uint32 type = first->_type;

      reclaim_item *item = first;
      first              = first->_next;

      switch (type)
	{
	case RECLAIM_FORMAT_BUFFER_HEADER:
	  {
	    UNUSED(item);
	    //pbf_reclaim *pbfr = (pbf_reclaim*) item;

	    //pbfr->_pb->print_format();
	  }
	  break;
	}
    }
}
