//
// Parses syntatic elements defined in RFC 2045
//

#ifndef M_PARSE2045_H
#define M_PARSE2045_H

#include <mail++/parse822.h>

class MRfc2045Parser : public MRfc822Tokenizer
{
private:
	typedef crope Rope;
	typedef crope::const_iterator Ptr;

public:
	int IsTSpecial(/*const*/ Ptr& p, const Ptr& e);
	int GetToken(Ptr& p, const Ptr& e, Rope& token);
	//
	// content = "Content-Type" ":" type "/" subtype *(";" parameter)
	//
	// For my own purposes I'm going to repartition the RFC syntax
	// into:
	//   content = type "/" subtype *parameter
	//   parameter = ";" attribute "=" value
	//
	int GetParameter(Ptr& p, const Ptr& e, Rope& attr, Rope& value);
	int GetContent(Ptr& p, const Ptr& e, Rope& type, Rope& subtype);
	int GetEncoding(Ptr& p, const Ptr& e, Rope& encoding);
};

#endif

