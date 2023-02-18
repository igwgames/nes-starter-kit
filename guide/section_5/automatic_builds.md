# Automatic Builds with CircleCI

Okay, so you've got a game you're constantly changing, and you want to put it on the web for everyone to see. Awesome!

But, how can you keep the web version up to date without constantly having to remember to upload it? And what if you
want to have older versions of your game ready for use? That's where continuous integration can come in. We choose
to do this with github's CI.

The good news is, this is already done on the main `nes-starter-kit` project, so adapting it for yourself should be pretty
simple. 

## The easy way, without s3

`nes-starter-kit` uses s3 to keep builds of the game around indefinitely, and make them publicly available. Without
that, you are limited to artifacts in github. These are readily available, but will be deleted after 90 days. They
are also stored in zip files, so they can't be directly embedded in an emulator.
 If that is all you need though, this approach is for you!

First, open up `.github/workflows/main.yml` - you should see a bunch of steps under a `build_rom` job. The last two
commands mention s3. Just delete those!. Now save, commit, push to Github, then open the artifacts tab in your 
Github project.

You should see your game building, or perhaps even built! Once it is done, you can see all of the
build output. There is also an "artifacts" section at the bottom, which will let you download your rom. That's it!

## The slightly less easy way, using s3

These steps will allow you to build your rom on every commit to your repository, and also link to it from the 
web, or an embedded emulator. It will require an AWS account, an s3 bucket, and some very light manual setup.

First, log into the AWS console, and create a new s3 bucket for your game. You will likely want to enable public
access to the bucket when you do. After this, open your newly created bucket and go to 
`Permissions` -> `CORS configuration` - there will be some default configuration here. This will allow you to
access your rom from web-based emulators. The default configuration works, so just hit `Save` on this page. 

You will also need to create an access key+secret in Amazon. That can be done by following 
[this guide](https://aws.amazon.com/premiumsupport/knowledge-center/create-access-key/). 

Next, go to the `Settings` tab for your Github project, then navigate to `Secrets and variables` on the left.
Under that, click `Actions`. You can ignore the `Environment secrets` section and create a `New repository` secret
at the top left. Name it `AWS_KEY_ID`, and put the key you generated above in it. 

Now repeat the same steps with a variable named `AWS_SECRET_ACCESS_KEY` and your secret key as a value. 

Finally, open `.github/workflows/main.yml` and change the last step to have your bucket name instead of `nes-starter-kit`.
You may also need to update the second-to-last step to use your rom name, instead of `nes-starter-kit-example.nes` 
Commit this, then push to Github. Your build should now start, and run all the way through.

If you look back at your s3 bucket, you should see these new files in it, in a folder named `main` (or
whatever branch you are working on.) That's it; you're done! You can link to these files on the web, and also
embed them onto a webpage using a web-based NES emulator. Enjoy!
