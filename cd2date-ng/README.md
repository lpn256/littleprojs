# cd2date-ng v5
Basic reverse engineering of cd2date by Aztekk. Should work identical to v2.0B1

Original can be found [here](https://archive.org/details/cd2date_v2.0B1).

	Usage: ./cd2date-ng -<manufacturer_type> <hubcode> "[decade]"
	Manufacturer Types:
	c - CMC Magnetics [A-Z][A-L][0-9][0-9] [decade]
	d - Daxon Technology [A-Z][A-Z][0-9][0-9]...[0-9][0-9]
	r - Type R [0-9][0-9][0-9][0-9][0-9] [decade]-
		Ritek Corporation, MJC Pte Ltd (Singapore),
		Optodisc Technology
	t - TDK Corporation (Luxembourg) [Early]
	g - Generic [0-9][0-9][0-9][0-9] [decade] -
		TDK Corporation (Luxembourg),
		Mitsubishi Chemical Corporation [CD-R],
		Mitsubishi Chemical Corporation [DVD-R], Taiyo Yuden,
		Mitsui Toatsu Chemicals + MAM-E, Moser Baer India,
		Prodisc, Eastman Kodak Company

	NOTES: Decade defaults to 2000 if not specified.
