# Baremetallics 

## About Us - Why we care
Embedded Systems are cool and fun. And more than anything else, easy.

But it's a sad fact that most of the content/information out there is not easy to process.
Specially for a beginner.

We are a community that wants to spread knowledge.
**Our goal is to make our content as easy as possible to understand.**

We don't want you to decipher tutorials, guides or books to learn.

## How Can You Take Part in the Community

### Spread the Word
Let people know about this proyect.
And how can they [support us](https://www.patreon.com/baremetallics).

### Write a Post
You can also write a post or tutorial. There are two ways to do it:

1. Send us an email:
Write a post in Markdown format and send us by mail to [support@baremetallics.com](support@baremetallics.com).
You can find the template with instructions [here](https://raw.githubusercontent.com/daleonpz/baremetallics/master/_drafts/2017-02-04-getting-started.md).

2. Create a pull request on Github.
Detailed instructions are not yet ready. But we'll post it soon ;).


### Improve the Website
Right now, there are no web-developers or graphic/UI-designers in our team.
Some of us have software development and marketing experience.
We try our best to give you the best experience.

**If you have ideas to improve this site.**
**We want to hear from you.**

**Don't be shy. Take Part**

We are sure that the comunnity will really appreciate it ;).

## Still Not Sure?
If you still don't know how you can help. No problem.
Send us a mail to [support@baremetallics.com](support@baremetallics.com)
And let's us know your interest! :)


## How to Run the Site Locally

1. Clone the repository

```bash
git clone https://github.com/daleonpz/baremetallics.git
```

2. Install ruby

```bash
sudo apt-get install ruby-full build-essential zlib1g-dev
```

3. Install Jekyll

```bash
gem install jekyll bundler
```

4. Install dependencies

```bash
bundle install
```

5. Run the site

```bash
bundle exec jekyll serve
```

6. Open your browser and go to `http://localhost:4000`

And that's it! You should see the site running locally.

An alternative is using Nix. If you have Nix installed, you can run `nix develop --ignore-environment` and then `bundle exec jekyll serve`.

### License
All this stuff is under the [MIT License](https://raw.githubusercontent.com/getmicah/getmicah.github.io/master/LICENSE)
