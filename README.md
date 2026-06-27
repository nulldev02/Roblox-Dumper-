# Roblox Dumper

This project was originally made for a private server. After the server shut down and I stopped actively working with Roblox exploits, I decided to release it publicly since I no longer have a use for it.

Keep in mind:

* Some offsets may not appear on the first run.
* Some signature patterns are more reliable than others.
* Running the dumper multiple times may produce better results.

## Features

* Dumps approximately 10–11 offsets
* Completes in around 5–10 seconds
* Automatically sends results to a Discord webhook
* Saves the dumped offsets to a local file

## Setup

1. Open `DiscordClient.cpp`.
2. Replace the existing webhook URL with your own Discord webhook.
3. Customize the embed title or other webhook settings if desired.

## Customization

You can personalize the project by editing:

* `Main.cpp` – Change the application name.
* `Dumper.cpp` – Modify the generated `Offsets.hpp` header comment (for example, replacing the default author text).

## Notes

This project is provided as-is. It was written for personal use and has not been maintained after its release. Some patterns may require updating as Roblox changes, and functionality may vary depending on the current version.
