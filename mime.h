//
// Mime Header Fields
//

#ifndef M_MIME_H
#define M_MIME_H

#include <mail++/parse822.h>

//
// Utility Functions
//

class MMessage;

int MSetMimeVersion(MMessage& message,
			int major = 1, int minor = 0);

int MSetContentType(MMessage& message,
			const crope& type, const crope& subtype);

//
// MIME-Version
//

class MMimeVersion
{
private:
	typedef crope	Rope;
	typedef crope::const_iterator	Ptr;

	int		major_;
	int		minor_;

	int Compare(const MMimeVersion&) const;

	Rope	text_;

	void Rewrite();

public:
	static int Write(Rope& text, int major, int minor);
	/**
	* Parses a character sequence for the major and minor number of
	* a MIME version field.
	*
	* @param p is a pointer to the beginning of the character sequence
	*          to read from, it will point past the last character read
	*          on success.
	* @param e is a pointer to the end of the character sequence.
	* @param major will be the major version number on success.
	* @param minor will bethe minor version number on success.
	* @return true on success, false on failure.
	*/
	static int Read(Ptr& p, const Ptr& e, int& major, int& minor);
	/**
	* Construct a Null-equivalent version object.
	*/
	MMimeVersion();
	/**
	* Construct the specified version object. Only 1.0 is valid
	* at the time of this writing.
	*/
	MMimeVersion(int major, int minor);
	/**
	* Construct a version object by reading a character sequence.
	* @see MMimeVersion::Read()
	*/
	MMimeVersion(Ptr& p, const Ptr& e);
	/**
	* Construct a version object by reading text's character sequence.
	*/
	MMimeVersion(const Rope& text);

	int Major() const;
	int Minor() const;

	const Rope& Text() const;
	
	int operator == (const MMimeVersion& r) const;
	int operator != (const MMimeVersion& r) const;
	int operator < (const MMimeVersion& r) const;
	int operator > (const MMimeVersion& r) const;
	operator const void* () const;
	operator ! () const;

	/**
	* The null version, it's major and minor are 0. Since the
	* first MIME version is 1.0, this is explicitly invalid.
	*/
	static const MMimeVersion Null;
};

//
// Parameter
//

class MParameter
{
private:
	typedef crope::const_iterator Ptr;
	typedef crope Rope;

	crope	name_;
	crope	value_;
	crope	text_;

	static int Write(Rope& text, const Rope& name, const Rope& value);
	static int Read(Ptr& begin, const Ptr& end, Rope& name, Rope& value);

public:
	MParameter();
	MParameter(const Rope& name, const Rope& value);
	MParameter(Ptr& begin, const Ptr& end);

	const Rope& Name() const;
	const Rope& Value() const;
	const Rope& Text() const;

	int operator == (const MParameter& r) const;
	int operator != (const MParameter& r) const;
	int operator < (const MParameter& r) const;
	int operator > (const MParameter& r) const;
	operator const void* () const;
	operator ! () const;

	/**
	* The null parameter, it's name is "".
	*/
	static const MParameter	Null;
};

//
// ContentType
//

class MContentType
{
private:
	typedef crope	Rope;
	typedef crope::const_iterator	Ptr;
	typedef vector<MParameter> Params;

	Rope	type_;
	Rope	subtype_;

	int EqualTo(const MContentType&) const;
	int True() const;

	Params params_;

	int Index(const Rope& name) const;

	Rope	text_;

	void Rewrite();

	static int Write(Rope& text, const Rope& t, const Rope& st, const Params& pl);
	static int Read(Ptr& p, const Ptr& e, Rope& t, Rope& st, Params& pl);

public:
	/**
	* Constructs a Null-equivalent version object.
	*/
	MContentType();
	/**
	* Constructs a field without paramaters.
	*/
	MContentType(const Rope& type, const Rope& subtype);
	/**
	* Constructs a field with paramaters.
	*/
	MContentType(const Rope& type, const Rope& subtype, const Params& params);
	/**
	* Construct by parsing a character sequence.
	*/
	MContentType(Ptr& p, const Ptr& e);
	/**
	* Construct by parsing a rope.
	*/
	MContentType(const Rope& text);

	const Rope&			Type() const;
	const Rope&			SubType() const;
	const MParameter&	Parameter(int index) const;
	const MParameter&	Parameter(const Rope& name) const;
	const MParameter&	Parameter(const Rope& name, const Rope& value);
	const Rope&			Text() const;

	int IsType(const crope& type) const { return type.compare(type_) == 0; }

	int operator == (const MContentType& r) const;
	int operator != (const MContentType& r) const;
	operator const void* () const;
	operator ! () const;

	/**
	* The null version, it's type and subtype are "".
	*/
	static const MContentType Null;
};

//
// MEncoding
//

class MEncoding
{
private:
	typedef crope	Rope;
	typedef crope::const_iterator	Ptr;

	Rope encoding_;
	Rope text_;

public:
	MEncoding();
	MEncoding(const Rope& encoding);
	MEncoding(Ptr& p, const Ptr& e);

	const Rope&	Encoding() const;
	const Rope&	Text() const;

	int operator == (const MEncoding& e);
	int operator != (const MEncoding& e);

	static const MEncoding E7Bit;
	static const MEncoding E8Bit;
	static const MEncoding EBinary;
	static const MEncoding EQuotedPrintable;
	static const MEncoding EBase64;
};

#endif

