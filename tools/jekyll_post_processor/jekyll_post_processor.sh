#!/bin/bash

echo "Starting Jekyll post-processor"
# Quick and dirty script to get our documentation into a format we can use with Jekyll
# Responsible for a few things: 
# 1. Copying all markdown to the /_docs folder
# 2. Adding the prefix code to all markdown
# 3. Generating the toc in _data/docs.yml
# 4. Moving all links from .md files to their .html counterparts.

# First, blow away the current docs folder; we want to replace it with anything new.
rm -rf _docs/*
# Build up the folder structure, so we can fill files into it in a few lines.
mkdir _docs/section_1
mkdir _docs/section_2
mkdir _docs/section_3
mkdir _docs/section_4
mkdir _docs/section_5
# Copy the images folder to the top level, so image links work from the Jekyll Site.
cp -R guide/images ./images
# Copy a couple files from the master branch, so we don't try to maintain them in the gh-pages branch
cp tools/jekyll_post_processor/_config.yml _config.yml
cp tools/jekyll_post_processor/index.html index.html
cp tools/jekyll_post_processor/favicon.ico favicon.ico

# Put together the pre-data for the README, so the page is in the right place.
READMEDATA="---
title: NES Starter Kit
permalink: /home.html
---"

# Copy the readme file alongside that prefix into the new documentation folder
(echo "$READMEDATA"; cat README.md) > _docs/README.md

# For every single markdown file in our guide...
for i in guide/**/*.md; do
	# Parse the name from the first line starting with a #
	TITLE=`grep "^#" $i | head -n1`
	# And take out the # and the space.
	TITLE=`echo ${TITLE:2}`
	
	# Find the location for the new file in our _docs folder.
	NEWFILE="${i/guide/_docs}"

	# And also what to name the file such that our links to it work.
	FILEURL="${NEWFILE/.md/.html}"
	FILEURL=${FILEURL/_docs/guide}

	# Create the pre-text for this chapter too.
	FULLHEAD="---
title: $TITLE
permalink: $FILEURL
---"

echo "Parsed $i as (title: $TITLE; link: $NEWFILE)"

# Output the file alongside its header text.
	(echo "$FULLHEAD"; cat $i) > $NEWFILE

done

# Okay, that handles the file updates. Now, for the table of contents...
# We parse the table of contents in the readme, then use this to build up the yaml 
# table of contents that our Jekyll site uses.
FILEDATA="$(cat ./README.md | sed -n '/<!-- TOC START -->/{:a;n;/<!-- TOC END -->/b;p;ba}')"
OUTFILE=""

# Now go through line-by-line to try to build this thing...
while read line; do

# If the line starts with ### it's a section name, so break that out.
if [[ $line == \#\#\#* ]]; then

# output a header for a new section, alongside a title.
OUTFILE="$OUTFILE
- title: \"${line/\#\#\#/}\"
  docs:
"

# Else, if it starts with (number). [link](), we parse that into a link
elif [[ $line =~ ^[[:digit:]]+\.[[:blank:]][[]*\]* ]]; then

# Extract the title and URL from the line
TITLE=$(echo "$line" | sed 's/](.*//' | sed 's/\[//' | sed 's/"/\\"/g')
URL=$(echo "$line" | sed 's/.*](//' | sed 's/)//g' | sed 's/.\/guide\//\/guide\//' | sed 's/.md/.html/' | sed 's/"/\\"/g')

# The way our regex works, if the thing isn't linked, the url and title will be the same, so we blank out the URL.
# The documentation site will show this as (coming soon) and not linked.
if [[ "$TITLE" == "$URL" ]]; then
URL=""
fi

# Add the yaml for thi title/url combiation.
OUTFILE="$OUTFILE
  - title: \"$TITLE\"
    url: \"$URL\"
"
fi

# Weird bash syntax stuff; this is the end of the loop, and where we finally tell it what file to read.
done <<< "$(echo -e "$FILEDATA")"

# Finally, output all of this into the docs.yaml file that Jekyll uses to generate the site.
echo "$OUTFILE" > _data/docs.yml

# ONE MORE THING! All of our guide links go to markdown files. 
# This tries to pull a find-and-replace on all of them.
# Hat tip: https://stackoverflow.com/questions/28402181/replace-many-arbitrary-markdown-links-with-grep-sed
find ./_docs -type f -exec sed -ri 's/([[]*]\(\S+).md\)/\1.html)/ig' {} \;

echo "Jekyll post-processor completed"