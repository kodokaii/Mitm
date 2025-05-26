# Mitm

Mitm is a simple command-line TCP proxy tool that acts as a "man-in-the-middle" between a client and a server. It allows you to intercept, observe, and relay traffic between both endpoints. This is useful for debugging, learning, or analyzing network protocols such as IRC.

## Features

- Transparent TCP proxying between a client and a server
- Logs all traffic in both directions with clear markers
- Useful for protocol analysis and debugging

## Build

To clone and build the project:

```sh
git clone git@github.com:kodokaii/Mitm.git --recursive
cd Mitm
make
```

This will fetch the repository (including its dependencies) and compile the `mitm` binary.

## Usage

```sh
./mitm <target_host> <target_port> <listen_port>
```

- `<target_host>`: The hostname or IP address of the server you want to connect to (e.g., `irc.libera.chat`)
- `<target_port>`: The port number of the server (e.g., `6667` for IRC)
- `<listen_port>`: The local port on which Mitm will listen for incoming client connections (e.g., `7777`)

## Example

Suppose you want to analyze IRC traffic to `irc.libera.chat` on port `6667`. You can run:

```sh
./mitm irc.libera.chat 6667 7777
```

Then, configure your IRC client to connect to `localhost` on port `7777` (without TLS). Mitm will relay all traffic between your client and the IRC server, logging the data as shown below:

```text
3< :copper.libera.chat NOTICE * :*** Checking Ident
3< :copper.libera.chat NOTICE * :*** Looking up your hostname...
3< :copper.libera.chat NOTICE * :*** No Ident response
3< :copper.libera.chat NOTICE * :*** Found your hostname: lfbn-rou-1-210-238.w83-114.abo.wanadoo.fr
[6]: connect
<span style="color:green">6> CAP LS 302</span>
<span style="color:green">6> JOIN :</span>
[6]: disconnect
3< :copper.libera.chat CAP * LS :account-notify away-notify chghost extended-join multi-prefix sasl=ECDSA-NIST256P-CHALLENGE,EXTERNAL,PLAIN,SCRAM-SHA-512 tls account-tag cap-notify echo-message server-time solanum.chat/identify-msg solanum.chat/oper solanum.chat/realhost
3< :copper.libera.chat 451 * :You have not registered
```

- Messages from the **server** (`<`) are shown in <span style="color:red">red</span>.
- Messages from the **client** (`>`) are shown in <span style="color:green">green</span>.

```

**Log format explanation:**

- `<` means the message comes from the server.
- `>` means the message comes from the client.
- The number before `<` or `>` is the socket file descriptor.
- Server messages are displayed in **red**.
- Client messages are displayed in **green**.
- `[fd]: connect` and `[fd]: disconnect` indicate connection events.

## Notes

- Make sure the listen port (`<listen_port>`) is not already in use.
- You may need to make the script executable: `chmod +x ./mitm`
- This tool is intended for educational and debugging purposes.
```
