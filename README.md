INSTALLATION:

# First, use the makefile to compile the program & set up the directory the program relies on in /usr/local/share
```
# make
```
# Lastly, install

```
# make install
```

# Now, you'll be able to run the program with the command 'bcc'

I should also note that at the moment, the "**Extras**" and "**Catamin Stages**" are not accessible in the stage menu (Selecting them will crash the program).
This is explained in the extremely choice words I have for the MyGamatoto developers in the comments of install_db.js

You may want to update the program with the following commands whenever MyGamatoto updates their database (I'll try to upload up to date versions when they come out, but here's how to manually update anyways)

***Fair warning: Updating enemies takes upwards of an hour and updating stages takes upwards of 8 hours***

# First, if you want to manually update the databases download the latest version of the database with:
```
$ node install_db.js
```
# Then, run the following command until it does NOT print any output
```
$ node stagefix.js
```
# Now, install the databases once more
```
# make install
```

#Notes:
You may need to install ncurses & jsoncpp
If you don't have them, the package names are libncurses-dev & libjsoncpp-dev
With apt, the command should be :
```
# apt install libncurses-dev
# apt install libjsoncpp-dev
```
You should just be able to use the install command with any other package manager and have it be the same -- I don't see a reason to suspect they'd change the package name when adding it to something like pacman or yum.
