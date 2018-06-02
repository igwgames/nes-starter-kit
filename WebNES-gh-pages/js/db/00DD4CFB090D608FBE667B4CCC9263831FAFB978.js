this.NesDb = this.NesDb || {};

NesDb[ '00DD4CFB090D608FBE667B4CCC9263831FAFB978' ] = {
	"$": {
		"name": "Indora no Hikari",
		"altname": "インドラの光",
		"class": "Licensed",
		"catalog": "KSC-IN",
		"publisher": "Kemco",
		"developer": "Kemco",
		"region": "Japan",
		"players": "1",
		"date": "1987-10-20"
	},
	"cartridge": [
		{
			"$": {
				"system": "Famicom",
				"crc": "174F860A",
				"sha1": "00DD4CFB090D608FBE667B4CCC9263831FAFB978",
				"dump": "ok",
				"dumper": "bootgod",
				"datedumped": "2011-09-08"
			},
			"board": [
				{
					"$": {
						"type": "HVC-SNROM",
						"pcb": "HVC-SNROM-04",
						"mapper": "1"
					},
					"prg": [
						{
							"$": {
								"name": "KSC-IN-0 PRG",
								"size": "128k",
								"crc": "174F860A",
								"sha1": "00DD4CFB090D608FBE667B4CCC9263831FAFB978"
							}
						}
					],
					"wram": [
						{
							"$": {
								"size": "8k",
								"battery": "1"
							}
						}
					],
					"vram": [
						{
							"$": {
								"size": "8k"
							}
						}
					],
					"chip": [
						{
							"$": {
								"type": "MMC1"
							}
						}
					]
				}
			]
		}
	]
};
