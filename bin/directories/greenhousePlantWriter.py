import tkinter as tk

def focus_next_window(event):
		event.widget.tk_focusNext().focus()
		return("break")

class Application(tk.Frame):
	def __init__(self, master = None):
		super().__init__(master)
		self.master = master
		self.pack()
		self.create_widgets()

	

	def create_widgets(self):
		self.frame2 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame2.pack()

		global unlockCheck
		unlockCheck = tk.IntVar()
		self.frame2.unlockCheck = tk.Checkbutton(self.frame2, height = 1, width = 1, variable = unlockCheck)
		self.frame2.unlockCheck.pack(side="left")
		self.frame2.unlockCheckL = tk.Label(self.frame2, width=15, text="unlocked if checked")
		self.frame2.unlockCheckL.pack(side="right")

		self.frame3 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame3.pack()
		self.frame3.nameL = tk.Label(self.frame3, width=15, text="name")
		self.frame3.nameL.pack(side = "left")
		self.frame3.nameI = tk.Text(self.frame3, height = 1, width = 20, relief = tk.RAISED)
		self.frame3.nameI.pack(side = "right")
		self.frame3.nameI.bind("<Tab>", focus_next_window)

		self.frame4 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame4.pack()
		self.frame4.boostNameL = tk.Label(self.frame4, width=15, text="boost name")
		self.frame4.boostNameL.pack(side = "left")
		self.frame4.boostNameI = tk.Text(self.frame4, height = 1, width = 20, relief = tk.RAISED)
		self.frame4.boostNameI.pack(side = "right")
		self.frame4.boostNameI.bind("<Tab>", focus_next_window)

		self.frame5 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame5.pack()
		self.frame5.healthL = tk.Label(self.frame5, width=15, text="health")
		self.frame5.healthL.pack(side = "left")
		self.frame5.healthI = tk.Text(self.frame5, height = 1, width = 20, relief = tk.RAISED)
		self.frame5.healthI.pack(side = "right")
		self.frame5.healthI.bind("<Tab>", focus_next_window)

		self.frame6 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame6.pack()
		self.frame6.defenseL = tk.Label(self.frame6, width=15, text="defense")
		self.frame6.defenseL.pack(side = "left")
		self.frame6.defenseI = tk.Text(self.frame6, height = 1, width = 20, relief = tk.RAISED)
		self.frame6.defenseI.pack(side = "right")
		self.frame6.defenseI.bind("<Tab>", focus_next_window)

		self.frame7 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame7.pack()
		self.frame7.attackL = tk.Label(self.frame7, width=15, text="attack")
		self.frame7.attackL.pack(side = "left")
		self.frame7.attackI = tk.Text(self.frame7, height = 1, width = 20, relief = tk.RAISED)
		self.frame7.attackI.pack(side = "right")
		self.frame7.attackI.bind("<Tab>", focus_next_window)

		self.frame8 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame8.pack()
		self.frame8.attackSpeedL = tk.Label(self.frame8, width=15, text="attack speed")
		self.frame8.attackSpeedL.pack(side = "left")
		self.frame8.attackSpeedI = tk.Text(self.frame8, height = 1, width = 20, relief = tk.RAISED)
		self.frame8.attackSpeedI.pack(side = "right")
		self.frame8.attackSpeedI.bind("<Tab>", focus_next_window)

		self.frame9 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame9.pack()
		self.frame9.speedL = tk.Label(self.frame9, width=15, text="speed")
		self.frame9.speedL.pack(side = "left")
		self.frame9.speedI = tk.Text(self.frame9, height = 1, width = 20, relief = tk.RAISED)
		self.frame9.speedI.pack(side = "right")
		self.frame9.speedI.bind("<Tab>", focus_next_window)

		self.frame10 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame10.pack()
		self.frame10.recoveryL = tk.Label(self.frame10, width=15, text="recovery")
		self.frame10.recoveryL.pack(side = "left")
		self.frame10.recoveryI = tk.Text(self.frame10, height = 1, width = 20, relief = tk.RAISED)
		self.frame10.recoveryI.pack(side = "right")
		self.frame10.recoveryI.bind("<Tab>", focus_next_window)

		self.frame11 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame11.pack()
		self.frame11.luckL = tk.Label(self.frame11, width=15, text="luck")
		self.frame11.luckL.pack(side = "left")
		self.frame11.luckI = tk.Text(self.frame11, height = 1, width = 20, relief = tk.RAISED)
		self.frame11.luckI.pack(side = "right")
		self.frame11.luckI.bind("<Tab>", focus_next_window)

		self.frame12 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame12.pack()
		self.frame12.levelL = tk.Label(self.frame12, width=15, text="level")
		self.frame12.levelL.pack(side = "left")
		self.frame12.levelI = tk.Text(self.frame12, height = 1, width = 20, relief = tk.RAISED)
		self.frame12.levelI.pack(side = "right")
		self.frame12.levelI.bind("<Tab>", focus_next_window)

		self.frame13 = tk.Frame(self, height = 1, bd = 2, relief = tk.RAISED)
		self.frame13.pack()
		self.frame13.experienceL = tk.Label(self.frame13, width=15, text="experience")
		self.frame13.experienceL.pack(side = "left")
		self.frame13.experienceI = tk.Text(self.frame13, height = 1, width = 20, relief = tk.RAISED)
		self.frame13.experienceI.pack(side = "right")
		self.frame13.experienceI.bind("<Tab>", focus_next_window)

		self.button = tk.Button(self)
		self.button["text"] = "write"
		self.button["command"] = self.writePlant
		self.button.pack(side="bottom")

	def writePlant(self):
		f = open("greenhouseUnlockFile.txt", "a")
		if unlockCheck.get():
			f.write('\n1')
		else:
			f.write('\n0')

		temp = []
		temp.append(self.frame3.nameI.get("1.0","end-1c"))
		temp.append(self.frame4.boostNameI.get("1.0","end-1c"))
		temp.append(self.frame5.healthI.get("1.0","end-1c"))
		temp.append(self.frame6.defenseI.get("1.0","end-1c"))
		temp.append(self.frame7.attackI.get("1.0","end-1c"))
		temp.append(self.frame8.attackSpeedI.get("1.0","end-1c"))
		temp.append(self.frame9.speedI.get("1.0","end-1c"))
		temp.append(self.frame10.recoveryI.get("1.0","end-1c"))
		temp.append(self.frame11.luckI.get("1.0","end-1c"))
		temp.append(self.frame12.levelI.get("1.0","end-1c"))
		temp.append(self.frame13.experienceI.get("1.0","end-1c"))

		self.frame3.nameI.delete("1.0", "end")
		self.frame4.boostNameI.delete("1.0", "end")
		self.frame5.healthI.delete("1.0", "end")
		self.frame6.defenseI.delete("1.0", "end")
		self.frame7.attackI.delete("1.0", "end")
		self.frame8.attackSpeedI.delete("1.0", "end")
		self.frame9.speedI.delete("1.0", "end")
		self.frame10.recoveryI.delete("1.0", "end")
		self.frame11.luckI.delete("1.0", "end")
		self.frame12.levelI.delete("1.0", "end")
		self.frame13.experienceI.delete("1.0", "end")

		for x in temp:
			x = x.rstrip("\n")
			print(repr(x))
			if x == "":
				f.write('\n0')
			else:
				f.write('\n' + x)

		f.close()

		self.frame3.nameI.focus_set()

def func(event):
	global app
	app.writePlant()

root = tk.Tk()
root.bind("<Return>", func)
app = Application(master = root)
app.master.title("greenhouseplant writer")
app.mainloop()



exit(0)