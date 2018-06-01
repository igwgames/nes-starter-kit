---
title: 
permalink: guide/section_5/automatic_builds.html
---
# Automatic Builds with CircleCI

Okay, so you've got a game you're constantly changing, and you want to put it on the web for everyone to see. Awesome!

But, how can you keep the web version up to date without constantly having to remember to upload it? And what if you
want to have older versions of your game ready for use? That's where continuous integration can come in. We choose
to do this to circleci. (Note: CircleCI requires your project to be public on Github.. private builds are possible, 
but are beyond the scope of this documentation.)

The good news is, this is done on the main `nes-starter-kit` project, so adapting it for yourself should be pretty
simple. 

## The easy way, without s3

`nes-starter-kit` uses s3 to keep builds of the game around indefinitely, and make them publicly available. Without
that, you are limited to what is shown in the circleci build logs when you're logged in. You can't (easily) access
these from the web, or embed them in an emulator. If that is all you need, this approach is for you!

First, open up `.circleci/config.yml` - you should see a bunch of steps under a `run` command. Two of the run commands
will be named "... in s3" - remve both of these run commands. Now save, commit, push to Github, then open the 
CircleCI website:

https://circleci.com/

Log in with Github, then find the projects page. Select your project name, and enable builds. Now make another commit
to your game and push. You should see your build start in the CircleCI ui. Once it is done, you can see all of the
build output. There is also an "artifacts" tab, which will let you download your rom. That's it!

## The slightly less easy way, using s3

These steps will allow you to build your rom on every commit to your repository, and also link to it from the 
web, or an embedded emulator. It will require an AWS account, an s3 bucket, and some manual setup and script editing.

First, log into the AWS console, and create a new s3 bucket for your game. You will likely want to enable public
access to the bucket when you do. After this, open your newly created bucket and go to 
`Permissions` -> `CORS configuration` - there will be some default configuration here. This will allow you to
access your rom from web-based emulators. The default configuration works, so just hit `Save` on this page. 

Next, go to the CircleCI website again, find your project, and click the cog to configure it. There should
be a section on the left named "AWS Permissions", which you can click into. Fill this out with credentials for
an AWS account with access to s3. (You can create one under `IAM` in the AWS console. There are guides online
if you need additional help with this.)

Finally, open `.circleci/config.yml` and change the two steps that mention `s3` to have your bucket name in
them. (That is, change `s3://nes-starter-kit/$CIRCLE_BRANCH/starter.latest.nes` to
`s3://your-s3-bucket-name/$CIRCLE_BRANCH/your_rom_name.nes`). Commit this, then push to Github. Your build
should now start, and run all the way through.

If you look back at your s3 bucket, you should see these new files in it, in a folder named `master` (or
whatever branch you are working on.) That's it; you're done! You can link to these files on the web, and also
embed them onto a webpage using a web-based NES emulator. Enjoy!
