﻿// Poker.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <string>
#include <iostream>
#include "GameData.h"

using namespace std;

const string INDENT =	" -";

const string FOLDS =	"folds";
const string RAISES =	"raises";
const string CALLS =	"calls";
const string CHECKS =	"checks";
const string BETS =		"bets";
const string DEALT =	"Dealt to";
const string HAS =		"has";
const string NEXTROUND=	"*** ";
const string UNCALLED =	"Uncalled bet";
const string COLLECTED=	"ollected ";
const string SEAT =		"Seat ";
const string INCHIPS =	" in chips";
const string SAID =		"said";
const string SB =		"posts small";
const string BB =		"posts big";

const string GAMESTATE[] = {"Preflop", "Flop   ", "Turn   ", "River  "}; // Used cosmeticly

const int PREFLOP =	0;
const int FLOP =	1;
const int TURN =	2;
const int RIVER =	3;
const int SHOWDOWN=	4;

// Skips characters until it finds the target character, stops at the character itself
void GameData::SkipToChar (string& source, int& pos, char c)
{
	while (source[pos] != c)
		pos++;
}
// Skips characters until it finds the target character, stops at the character after the target
void GameData::SkipOverChar (string& source, int& pos, char c)
{
	SkipToChar(source, pos, c);
	pos++;
}
// Copies all characters to the destination string until it finds the target character
void GameData::WriteToChar (string& source, string& dest, int& pos, char c)
{
	string s;
	while (source[pos] != c)
	{
		s = source[pos];
		dest.append(s);
		pos++;
	}
}
// Customized version of writeToChar, because names can have stuff like (, ) and $
void GameData::ParsePlayerName (string& source, string& dest, int& pos)
{
	string s;
	while (true)
	{
		WriteToChar(source, dest, pos, '$');
		int p = pos;
		SkipToChar(source, p, ' ');
		if (CheckString(source, p, INCHIPS)) // If the $ was not in the player name; done
			break;
		s = source[pos];
		dest.append(s);						// Else it was part of the player name; add it and continue
		pos++;
	}
	TrimStringEnd(dest, 2);
}
// Skips all characters until the next line, stops at the first character after the new line
void GameData::SkipToNextLine(string& source, int& pos)
{
	SkipOverChar(source, pos, '\n');
}

// Skips al characters until the next game, stops at the first character of the new game
bool GameData::SkipToNextGame(string& source, int& pos)
{
	bool ret = false;
	bool currentNL = false;
	bool previousNL = false;
	bool foundGap = false;
	for (; pos < source.length(); pos++)
	{
		currentNL = (source[pos] == '\n');
		if (currentNL && previousNL)
			foundGap = true;
		if (foundGap && !currentNL)
			return ret;
		previousNL = currentNL;
	}
	ret = true;
	return ret;
}
// Just jump a couple characters, either positive or negative
void GameData::JumpPos (int& pos, int amount)
{
	pos += amount;
}
// Skips the length of the string, plus 2 additional characters (it just happens to be the right amount for the distance between the player name and the action, but also between the action and (if applicable) amount
void GameData::SkipString (string& source, string str, int& pos)
{
	JumpPos(pos, str.length() + 2);
}
// Shortens a string by the set amount
void GameData::TrimStringEnd (string& s, int amount)
{
	s.resize(s.length() - amount);
}
// Checks if the next characters in the source string are identical to the match string
bool GameData::CheckString(string& source, int& pos, string match)
{
	int l = match.length();
	bool ret = false;
	for (int i = 0; i < l; i++)
		if (source[pos + i] != match[i])
			return ret;
	ret = true;
	return ret;
}
// Reads the player name and returns the position the player position in the order in which they were first read.
// Its intended use is to find out who's the sb, to re-order the player list, so it's only used once.
int GameData::ReadPlayerName(string& source, int& pos, string players[])
{
	int i;
	for (i = 0; i < pAmount; i++)
	{
		if (CheckString(source, pos, players[i]))
			return i;
	}
	i = -1;
	return i;
}
// Advances the player position, and makes sure to handle overflow
void GameData::AdvancePlayerPosition(int& playerPos)
{
	playerPos++;
	if (playerPos == pAmount)
		playerPos = 0; // You can write it like p = (p + 1) % pAmount, but it will take more CPU cycles
}
// Converts a string to pennies, so we won't have to deal with floats
int GameData::StrToPennies(string str)
{
	int ret;
	int l = str.length();
	for (int i = 0; i < l; i++)
	{
		if (str[i] == '.') 
		{
			ret = atoi(str.replace(i, 1, "").c_str());
			return ret;
		}
	}
	ret = atoi(str.c_str()) * 100;
	return ret;
}
// Sets up the player structures, which will be used to store moves
void GameData::SetupPlayerStructs(string players[], string pCash[], int& playerPos)
{
	for (int i = 0; i < pAmount; i++)
	{
		playerData[i].name = players[playerPos];
		playerData[i].money = StrToPennies(pCash[playerPos]);
		AdvancePlayerPosition(playerPos);
	} // end for
}
// Skips players that are inactive, won't increase the player position initially
void GameData::FindNextPlayer(int& pPos, int gameState, int& movesCounter, bool active[])
{
	while (!active[pPos])
	{
		AdvancePlayerPosition(pPos);
		cout << " ";
		moveData[movesCounter][gameState].move = SKIP;
		movesCounter++;
	}
}
// Adds an action to all players and moves player to next position
void GameData::AddAction(int& pPos, Move action, int gameState, int& movesCounter, bool active[], int betAmount, int pot)
{
	moveData[movesCounter][gameState].move = action;
	moveData[movesCounter][gameState].pot = pot;
	movesCounter++;

	if (action == FOLD)
		active[pPos] = false;

	AdvancePlayerPosition(pPos);

	switch (action)
	{
		case FOLD:
			cout << 'f';
			break;
		case CALL:
			cout << 'c';
			break;
		case RAISE:
			cout << 'r';
			break;
	}

	FindNextPlayer(pPos, gameState, movesCounter, active);
}
// Returns the amount until character
int GameData::ReadAmount(string& source, int& pos, char c)
{
	string amountStr;
	WriteToChar(source, amountStr, pos, '\n');
	int betAmount = StrToPennies(amountStr);
	return betAmount;
}

// Main function that parses through the entire source file
void GameData::ParseAll(string& source, int& pos)
{
	SkipOverChar(source, pos, '#');				
	WriteToChar(source, gameID, pos, ':');		
	
	cout << "Parsing game #" << gameID << ":";

//	if (gameID == "75956039754")
//		cout << "";

	SkipOverChar(source, pos, 'm');
	JumpPos(pos, 1);
	WriteToChar(source, limitType, pos, '(');
	TrimStringEnd(limitType, 1);
	JumpPos(pos, 2);
	{
		string s;
		WriteToChar(source, s, pos, '/');
		sb = StrToPennies(s);
	}
	JumpPos(pos, 2);
	{
		string s;
		WriteToChar(source, s, pos, ' ');
		bb = StrToPennies(s);
	}
	JumpPos(pos, 3);							
	SkipOverChar(source, pos, '\'');
	SkipOverChar(source, pos, '\'');
	SkipOverChar(source, pos, ' ');	
	WriteToChar(source, gameType, pos, ' ');
	SkipToNextLine(source, pos);

	{
		string players	[MAXPLAYERS];			// We don't know the amount of players yet, this array is temporarily
		string pCash	[MAXPLAYERS];
		{
			int i = 0;
			while (CheckString(source, pos, SEAT))
			{
				JumpPos(pos, 8);					//
				ParsePlayerName(source, players[i], pos);
				JumpPos(pos, 1);					//
				WriteToChar(source, pCash[i], pos, ' ');
				SkipToNextLine(source, pos);
				i++;
			} // end while
			pAmount = i;							// Player amount
		} // end i

		int playerPos;
		bool found = false;
		do {
			playerPos = ReadPlayerName(source, pos, players);	// Relative to the order of how the players were parsed, becomes absolute after setPlayerPositions() is used
			SkipString(source, players[playerPos], pos);
			found = CheckString(source, pos, SB);
			SkipToNextLine(source, pos);
		}
		while (!found);											// Skips until it finds the SB
		
		SetupPlayerStructs(players, pCash, playerPos);			// Orders the players correctly, so 0 = sb, 1 = bb, ..
	} // end players[], pCash[], playerPos, pPosition[], found
	SkipToNextLine(source, pos);
	while (!CheckString(source, pos, NEXTROUND))
	{
		SkipToNextLine(source, pos);
	}
	/*while (CheckString(source, pos, DEALT))
	{
		SkipOverChar(source, pos, '[');			// Dealt to Hero [
		WriteToChar(source, playerData[playerPos].card1, pos, ' ');	// Jc
		JumpPos(pos, 1);						//  
		WriteToChar(source, playerData[playerPos].card2, pos, ']');	// Kd
		SkipToNextLine(source, pos);
	} // end while*/
	{
		int playerPos;
		int pot = 0;
		bool active[MAXPLAYERS];
		for each (bool b in active)
			b = true;					// Every player is active

		SkipToNextLine(source, pos);
		{
			int gameState = 0;
			while (gameState < SHOWDOWN)
			{
				cout << endl << INDENT << "" << GAMESTATE[gameState] << ": ";

				int movesCounter = 0;

				if (gameState == 0)
				{
					cout << "SB";									// Indicating small blind and big blind for player 0 and 1
					playerPos = 2;									// Skipping the sb and bb, sb and bb is a rule, not a player's strategy, so no point in including them
				}
				else
				{
					playerPos = 0;
					FindNextPlayer(playerPos, gameState, movesCounter, active);				// Finds the first player, if it's the first round, leaves it to 2
				}

				while (!CheckString(source, pos, NEXTROUND))		// Do this until the next round
				{
					if (!CheckString(source, pos, UNCALLED))		// If it's not something that we don't handle yet
					{												// TODO: When handling bet amounts, take this into account
						if (CheckString(source, pos, playerData[playerPos].name))  // If the correct player's name is found, check what action he did
						{
							SkipString(source, playerData[playerPos].name, pos);
							if (CheckString(source, pos, FOLDS))					// Folds
							{
								AddAction(playerPos, FOLD , gameState, movesCounter, active, 0, 0);
							}
							else if (CheckString(source, pos, CALLS))				//Calls
									 
							{
								SkipString(source, CALLS, pos);
								
								AddAction(playerPos, CALL , gameState, movesCounter, active, betAmount, pot);
								pot += betAmount;
							}
							else if (CheckString(source, pos, CHECKS))				// Checks
							{
								AddAction(playerPos, CHECK , gameState, movesCounter, active, 0, pot);
							}
							else if (CheckString(source, pos, RAISES))				// Raises
							{
								SkipString(source, RAISES, pos);
								string amountStr;
								WriteToChar(source, amountStr, pos, ' ');
								int betAmount = StrToPennies(amountStr);
								AddAction(playerPos, CALL , gameState, movesCounter, active, betAmount, pot);
								pot += betAmount;
							}
							else if	(CheckString(source, pos, BETS))
							{

								AddAction(playerPos, RAISE, gameState, movesCounter, active);
							}
							else if (CheckString(source, pos, COLLECTED))			// Collected, game ends
							{
								gameState = SHOWDOWN;
								cout << "W";
								break;
							}
							else
							{
								// Ignore action
							}
						}
					}
					SkipToNextLine(source, pos);
				}
				gameState++;

				// Positions the position to the first card they need
				switch (gameState)
				{
				case RIVER:
					SkipOverChar(source, pos, '[');
				case TURN:
					SkipOverChar(source, pos, '[');
				case FLOP:									// Could be replaced by default: to speed it up, but it's unsafe
					SkipOverChar(source, pos, '[');
				}

				// Writes the cards to the correct place
				switch (gameState)
				{
				case RIVER:
					WriteToChar(source, communityCards[4], pos, ']');
					break;
				case TURN:
					WriteToChar(source, communityCards[3], pos, ']');
					break;
				case FLOP:									// Could be replaced by default: to speed it up, but it's unsafe
					WriteToChar(source, communityCards[0], pos, ' ');
					JumpPos(pos, 1);
					WriteToChar(source, communityCards[1], pos, ' ');
					JumpPos(pos, 1);
					WriteToChar(source, communityCards[2], pos, ']');
				}

				SkipToNextLine(source, pos);
			}
		} // end playerPos
	}
	cout << endl << endl;

	last = SkipToNextGame(source, pos);
	system("PAUSE");
}

GameData::GameData (string& source, int& pos)
{
	ParseAll(source, pos);
}