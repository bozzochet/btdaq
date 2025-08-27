#For the maintainers:

##To update the submodules

```
git submodule foreach git pull origin master
```

and then commit

(I also wrote down it:

```
git submodule update --remote --merge
```

are the two equivalent?)

# For the users:

## First clone

```
git pull
```

this will create, inside the Decode dir a dir called deps. But this will be empty

```
git submodule init
```

this must be done only first time.

```
git submodule update
```

every time I change something on the submodule.
Another option for the first time is

```
git clone --recurse-submodules https://blablabla
```

or, after the pull/clone

```
git submodule update --init --recursive
```

# Updates

```
git submodule update
```

or maybe better

```
git pull --recurse-submodules
```