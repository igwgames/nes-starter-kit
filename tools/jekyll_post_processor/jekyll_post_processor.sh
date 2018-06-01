#!/bin/bash

# Quick and dirty script to get our documentation into a format we can use with Jekyll
# Responsible for a few things: 
# 1. Copying all markdown to the /_docs folder
# 2. Adding the prefix code to all markdown
# 3. Generating the toc in _data/docs.yml

# Format is: 
#---
#title: This is The Title
#keywords: introduction
#permalink: /docs/section_1/setting_up_your_tools.html
#---

rm -rf _docs/*
# cp -R guide/* _docs
mkdir _docs/section_1
mkdir _docs/section_2
mkdir _docs/section_3
mkdir _docs/section_4
mkdir _docs/section_5
cp -R guide/images ./images
cp tools/jekyll_post_processor/_config.yml _config.yml
cp tools/jekyll_post_processor/index.html index.html
cp tools/jekyll_post_processor/favicon.ico favicon.ico
#cp README.md _docs/README.md

READMEDATA="---
title: NES Starter Kit
permalink: /home.html
---"

(echo "$READMEDATA"; cat README.md) > _docs/README.md

for i in guide/**/*.md; do
	TITLE=`grep "^#" $i | head -n1`
	TITLE=`echo ${title:2}`
	NEWFILE="${i/guide/_docs}"
	FILEURL="${NEWFILE/.md/.html}"
	FILEURL=${FILEURL/_docs/guide}
	FULLHEAD="---
title: $TITLE
permalink: $FILEURL
---"

	(echo "$FULLHEAD"; cat $i) > $NEWFILE

done

# Okay, that handles the file updates. Now, for the table of contents...
FILEDATA="$(cat ./README.md | sed -n '/<!-- TOC START -->/{:a;n;/<!-- TOC END -->/b;p;ba}')"
OUTFILE=""

# Now go through line-by-line to try to build this thing...
while read line; do

if [[ $line == \#\#\#* ]]; then
OUTFILE="$OUTFILE
- title: \"${line/\#\#\#/}\"
  docs:
"
elif [[ $line =~ ^[[:digit:]]+\.[[:blank:]][[]*\]* ]]; then
TITLE=$(echo "$line" | sed 's/](.*//' | sed 's/\[//' | sed 's/"/\\"/g')
URL=$(echo "$line" | sed 's/.*](//' | sed 's/)//g' | sed 's/.\/guide\//\/guide\//' | sed 's/.md/.html/' | sed 's/"/\\"/g')
if [[ "$TITLE" == "$URL" ]]; then
URL=""
fi
OUTFILE="$OUTFILE
  - title: \"$TITLE\"
    url: \"$URL\"
"
fi
done <<< "$(echo -e "$FILEDATA")"

echo "$OUTFILE" > _data/docs.yml

# ONE MORE THING! All of our guide links go to markdown files. 
# This tries to pull a find-and-replace on all of them.
# Hat tip: https://stackoverflow.com/questions/28402181/replace-many-arbitrary-markdown-links-with-grep-sed
# find ./_docs -type f -exec sed -ri 's/\b(+\[[^\]]*\]\()[^()]*\)/\1foo.bar.com)/g' {} \;
find ./_docs -type f -exec sed -ri 's/([[]*]\(\S+).md\)/\1.html)/ig' {} \;
