#ifndef __DUMP_LIST_HH__
#define __DUMP_LIST_HH__

#include <vector>
#include <assert.h>
#include <string.h>

template<typename item>
void dump_list(const std::vector<item*> *v,dumper &d,const char *separator)
{
  if (!v)
    return;

  typename std::vector<item*>::const_iterator i;

  i = v->begin();

  if (i != v->end())
    {
      (*i)->dump(d);
      ++i;

      for (; i != v->end(); ++i)
	{
	  if (strcmp(separator,"\n") == 0)
	    d.nl();
	  else
	    d.text(separator);	

	  (*i)->dump(d);
	}
      if (strcmp(separator,"\n") == 0)
	d.nl();
    }
}

template<typename item_list>
void dump_list_paren(const item_list *v,dumper &d,const char *delim,bool first_comma = false)
{
  assert(delim[0] && delim[1]);

  d.text_fmt("%c",delim[0]); // '('
      
  if (v)
    {
      dumper sd(d);
      
      typename item_list::const_iterator i;
      
      i = v->begin();
      
      if (i != v->end())
	{
	  if (first_comma)
	    sd.text(",");
	  (*i)->dump(sd);
	  ++i;
	  
	  for (; i != v->end(); ++i)
	    {
	      sd.text(",");		      
	      (*i)->dump(sd);
	    }
	}
    }
  d.text_fmt("%c",delim[1]); // ')'
}

/*
template<typename item>
void dump_list_braces(const std::vector<item*> *v,dumper &d)
{
  d.text("{");
  d.nl();
      
  if (v)
    {
      dumper sd(d,2);
      
      typename std::vector<item*>::const_iterator i;
      
      for (i = v->begin(); i != v->end(); ++i)
	{
	  (*i)->dump(sd);
	  sd.nl();
	}
    }
  d.text("}");
}
*/
template<typename item_list>
void dump_list_braces(const item_list *v,dumper &d,bool end_brace = true,bool start_brace = true)
{
  if (start_brace)
    d.text("{\n");
      
  if (v)
    {
      dumper sd(d,2);
      
      typename item_list::const_iterator i;
      
      for (i = v->begin(); i != v->end(); ++i)
	{
	  (*i)->dump(sd);
	  sd.nl();
	}
      if (v->empty())
	sd.text(";\n");
    }
  if (end_brace)
    d.text("}");
}

template<typename item_list>
void dump_list_args(const item_list *v,dumper &d,const char *type_name)
{
  if (v)
    {
      dumper sd(d);
      
      typename item_list::const_iterator i;
      
      for (i = v->begin(); i != v->end(); ++i)
	{
	  sd.text_fmt(",%s ",type_name);		      
	  (*i)->dump(sd);
	}
    }
}

#endif//__DUMP_LIST_HH__
