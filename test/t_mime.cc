//
// Tests components of mime parsing.
//

#include <assert.h>

#include <mail++/mime.h>

typedef crope Rope;
typedef crope::const_iterator Ptr;

void t_version()
{
	struct Version {
		const char* s;
		int major;
		int minor;
	};
	Version versions[] = {
		{ " () 1 () . 3 () ", 1, 3 },
		{ " (1.2) 3 (1.2) . 0 (1.2) ", 3, 0 },
		{ "1 () . 3 () ", 1, 3 },
		{ "1.0", 1, 0 },
		{ 0, 0, 0 }
	};

	for(int i = 0; versions[i].s; ++i)
	{
		Rope s = versions[i].s;
		Ptr p = s.begin();
		Ptr e = s.end();

		MMimeVersion mv(p, e);

		cout << mv.Text() << " <-- '" << versions[i].s << "'" << endl;

		if(mv.Major() != versions[i].major ||
			mv.Minor() != versions[i].minor)
		{
			cout << "^--> failed!" << endl;
		}
	}
}
void t_content()
{
	const char* types[] = {
		"audio/basic; name=VoiceMail_beatty0.vox",
		"image/x-xbitmap",
		"message/rfc822",
		"Message/External-body;",
		"MULTIPART/mixed; BOUNDARY=\"-559023410-2078917053-946053953=:10725\"",
		"Multipart/Mixed; Boundary=3D\"NextPart\"",
		"multipart/mixed; boundary=\"------------3B62D02BA9B10FA8F1A252D7\"",
		"multipart/mixed; boundary=\"mail.sleepy.sau.135.1476\"",
		"multipart/mixed; boundary=Where_No_Man_Has_Gone_Before",
		"multipart/mixed; boundary=foobarbazola",
		"multipart/mixed; boundary=n2m-MIME-boundary----------",
		"multipart/signed; micalg=pgp-md5; protocol=\"application/pgp-signature\"; boundary=\"DKU6Jbt7q3WqK7+M\"",
		"APPLICATION/OCTET-STREAM",
		"application/pgp-signature",
		"text/plain",
		"TEXT/PLAIN; CHARSET=US-ASCII",
		"text/plain; charset=ISO-2022-JP",
		"text/plain; charset=US-ASCII",
		"text/plain; charset=iso-8859-1",
		"text/plain; charset=us-ascii",
		"text/plain; type=microsoft-rtf; name=index.rtf",
		"text/richtext",
		"text/richtext; charset=US-ASCII",
		"text/x-vcard; charset=us-ascii; name=\"vcard.vcf\"",
		0
	};
	for(int i = 0; types[i]; ++i)
	{
		Rope s = types[i];
		Ptr p = s.begin();
		Ptr e = s.end();

		MContentType ct(p, e);

		cout << "Content-Type: " << s << endl;

		if(ct) {
			cout
				<< "--> type: " << ct.Type() << " "
				<< "subtype: " << ct.SubType() << endl;
			for(int j = 0; ct.Parameter(j); ++j) {
				cout
					<< "--> param attr: " << ct.Parameter(j).Name()
					<< " value: " << ct.Parameter(j).Value() << endl;
			}
		} else {
			cout << "failed" << endl;
		}
	}
}

void t_encode_check(const char* e, const MEncoding& check, int ok = 1)
{
	MEncoding enc(e);

	cout << enc.Encoding() << " --> " << enc.Text();

	if((enc == check) != ok)
		cout << ": failed!";

	cout << endl;
}

void t_encoding()
{
	t_encode_check("8bit ",	MEncoding::E8Bit);
	t_encode_check(" () 7BIT ", MEncoding("7bIt"));
	t_encode_check("()7bit ", MEncoding::E7Bit);
	t_encode_check("() base64 ", MEncoding::EBase64);
	t_encode_check(" binary ", MEncoding::EBinary);
	t_encode_check(" quoteD-Printable ", MEncoding::EQuotedPrintable);
	t_encode_check(" ()X-uuencoded ", MEncoding("uuencoded"), 0);
}

int main()
{
	//t_version();
	t_content();
	t_encoding();

	return 0;
}

