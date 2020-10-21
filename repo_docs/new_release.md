# Crafting a new release

**Note**: Yeah, a lot of this could be automated pretty easily. With how rare it is, I haven't put in the effort yet. If
that changes, it may make sense to put a nice script into the tools dir.

1. Determine the new version number. Use semantic release patterns. 
2. Determine if there were any tools changes. If not, skip the next step.
3. IF CHANGED, Bump `package.json` in all of the helper node modules. There should be 3.
4. IF CHANGED, Run builds for all of the helper modules using `make rebuild_tools`
5. Update version number(s) at the top of `README.md` to match the version you created
6. IF CHANGED, update the build tools number in `readme.md` as well as in `tools/tool_zip/readme.txt`
7. Commit and push to master
8. Create a new version through the Github ui, filling out the changes based on recent Git history (Yes, we want a tag.)
9. For every branch based off a guide chapter, run the following: 
    ```bash
    git fetch
    git checkout BRANCHNAME
    git rebase origin/master
    # If there are any conflicts, or this change is big, run `make clean && make && make run` then test the rom.
    git push --force origin BRANCHNAME
    ```
10. IF CHANGED, Build a new zip file using `make build_tool_zip` and upload this to `cpprograms.net`, replacing the old
   one. (The old one should be renamed with its version number; IE `nes_starter_kit_tools.1.0.2.zip`)
11. IF ANY BREAKING CHANGES WERE MADE, update the README and state them, as well as the version people can go back to.

## Potential Improvements

- This process could very well be automated - especially the building of the modules.
- The branch thing is annoying and repetitive, but also dangerous. Can we automate without taking away control?
