<p align="center">
  <a href="" rel="noopener">
 <img width=150px height=150px src="prinfo/assets/images/prinfo.png" alt="prinfo logo"></a>
</p>

<h3 align="center">prinfo</h3>

<div align="center">

[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![Downloads](https://img.shields.io/github/downloads/nikoksr/prinfo/total)]()
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](/LICENSE)

</div>

---

<p align="center"> Fast printer information gathering tool for windows.
    <br>
</p>

## Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)
- [TODO](../TODO.md)
- [Contributing](../CONTRIBUTING.md)

## About <a name = "about"></a>

Gather all relevant informations about the printers in your network in one place with blazing speed. Using the winapi, reading the registry and analyzing printer relevant folders, prinfo displays all the important informations about your printers in seconds.

## Getting Started <a name = "getting_started"></a>

### Installing

Download the latest [release](https://github.com/nikoksr/prinfo/releases) for your system architecture. A single executable, that's all.

**Note**: This is currently a windows only project.

## Usage <a name="usage"></a>

### Main menu
Using prinfo is pretty straight forward. Executing it, you're greated by a simple but classy cli menu.

<p align="left">
  <a href="" rel="noopener">
 <img src="assets/images/main-menu.png" alt="prinfo main menu"></a>
</p>

- Option 1 gives a you a quick overview of the printers in your network.
- Option 2 opens a new menu that includes a variety of printer-related functions.
- Option 3 displays the help.

### Functions menu

A list of functions that either gather and display informations or try to analyze and help you identify potential problems.

<p align="left">
  <a href="" rel="noopener">
 <img src="assets/images/functions-menu.png" alt="prinfo functions menu"></a>
</p>

Those are the functions that are executable by the **regular user**.

- Option 1 shows you the relevant informations about your printers that's accesible through the winapi.
- Option 2 shows you relevant informations read from the registry.
- Option 3 shows you informations about the system you're currently using which might also help identifying problems related to printing.
- Option 4 quickly save the most important infos to a text file.

Here begins the **admin area**. It's is hidden from the regular user by default. Functions in this area require the access rights of an admin.

- Option 5 analyzes the printers folder. Check for empty spooler files and incomplete file pairs and show files that shouldn't be in there. A lot of printer problems originate from here. Broken, empty and incomplete spooler files can prevent the spooler from working properly.
- Option 6 quickly restarts the spooler service.

By default every information displaying function includes the option to save the results to a file.
