/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eandre-f <eandre-f@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 12:09:40 by eandre-f          #+#    #+#             */
/*   Updated: 2023/06/10 17:49:51 by eandre-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "log_utils.hpp"
#include "utils.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

typedef pair<string, string> pair_string_t;
typedef list<pair_string_t>  labels_t;

#define FAILURE 1

class Config
{
  public:
	Config(void);
	~Config(void);

	int add(string label, string value);

	vector<int> const &getPorts(void) const;

	static string         readFile(const string &filename);
	static vector<Config> parseConfig(string &filedata);

  private:
	vector<int> _ports;
	int         _setPorts(string &port);
};

#endif /* CONFIG_HPP */
