# Crafting a new release

**Note**: Yeah, a lot of this could be automated pretty easily. With how rare it is, I haven't put in the effort yet. If
that changes, it may make sense to put a nice script into the tools dir.

1. Determine the new version number. Use semantic release patterns. 
2. Determine if there were any tools changes. If so, make sure `.create-nes-game.config.json` is updated w/ new versions. 
3. Update version number(s) at the top of `README.md` to match the version you created
4. IF CHANGED, update the required create-nes-game version in `README.md`
5. Commit and push to `main`
6. Create a new version through the Github ui, filling out the changes based on recent Git history (Yes, we want a tag.)
7. For every branch based off a guide chapter, run the following: 
    ```bash
    git fetch
    git checkout BRANCHNAME
    git rebase origin/main
    # If there are any conflicts, or this change is big, run `create-nes-game clean && create-nes-game build && create-nes-game run` then test the rom.
    git push --force origin BRANCHNAME
    ```
8. IF ANY BREAKING CHANGES WERE MADE, update the README and state them, as well as the version people can go back to.

## Potential Improvements

- This process could very well be automated - especially the building of the modules.
- The branch thing is annoying and repetitive, but also dangerous. Can we automate without taking away control?
