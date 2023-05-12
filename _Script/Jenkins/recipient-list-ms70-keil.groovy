<%  
    def ip_owner_list = [
//			IP						Owner/Co-owners
			['acmp',				'CYTseng1'		],
			['adc',					'CYTseng1'		],
		   // eadc
			['canfd',				'CYTseng1', 	],
			['clk',					'MCChang2',		],
			['crypto',				'CYHsu11'		],
			['crc',					'YCHuang12'		],
			['dac',					'CYTseng1'		],
			['fmc',					'KLChiu0',		],
			['gpio',				'CCWang20', 	],
			['hard_fault_sample',	'KLChiu0',		],
			['i2c',					'KLChiu0',		],
		   // usci_i2c
			['i2s',					'CCWang20',		],
		   // spii2s
			['isp',					'KLChiu0',		],
			['lcd',					'MCChang2'		],
			['led_toggle',			'CCWang20',		],
			['mpu',					'CYHsu11',		],
			['opa',					'CYTseng1'		],
			['pdma',				'KLChiu0',		],
			['psio',				'CCWang20',		],
			['pwm',					'YCHuang12',	],
		   // bpwm
			['rtc',					'CYHsu11', 		],
			['sc',					'CYHsu11',		],
		   // scuart
			['semihost',			'KLChiu0',		],
			['spi',					'CCWang20',		],
		   // qspi
		   // usci_spi
			['sys',					'MCChang2',		],	// trim
			['template',			'KLChiu0',		],
			['timer',				'YCHuang12'		],
		   // timer_pwm
			['tk',					'CYHsu11'		],
			['usbd',				'CYTseng1', 	],
			['xom',					'KLChiu0',		],
			['wdt',					'CYHsu11', 		],
		   // wwdt
			['uart',				'CYTseng1',		]
		   // usci_uart
	]
    def recipient_list = ""
    def cc_list = ""
 
    if (build.workspace.isRemote())
    {
        channel = build.workspace.channel;
        fp = new FilePath(channel, build.workspace.toString() + "/keil.txt")
    } else {
        fp = new FilePath(new File(build.workspace.toString() + "/keil.txt"))
    }

    if (fp != null)
    {
        f = new File(fp.toURI())

        f.eachLine() { line->
            // Check IP
            for (i in 0..ip_owner_list.size()-1)
            {
				if (line.toLowerCase().indexOf(ip_owner_list[i][0]) >= 0)
				{
					// Add owner to recipient_list and remove from cc_list
					if (recipient_list.indexOf(ip_owner_list[i][1]) < 0)
					{
						recipient_list += (ip_owner_list[i][1] + "@nuvoton.com,")
						cc_list = cc_list.replaceAll("cc:" + ip_owner_list[i][1] + "@nuvoton.com,", "")
					} // if

					// Add co-owners to cc_list
					for (j in 2..ip_owner_list[i].size()-1)
						if (cc_list.indexOf(ip_owner_list[i][j]) < 0 && recipient_list.indexOf(ip_owner_list[i][j]) < 0)
							cc_list += ("cc:" + ip_owner_list[i][j] + "@nuvoton.com,")
					
					// Only first match
					break
				} // if
            } // for i
        } // f.eachLine()
    } // if

	if (recipient_list.size() == 0)
		recipient_list = "CHHsieh3@nuvoton.com"
	else
		cc_list += "cc:CHHsieh3@nuvoton.com"
    recipient_list += cc_list
%>

${recipient_list}
