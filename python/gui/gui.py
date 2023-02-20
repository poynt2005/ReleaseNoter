import tkinter as tk
from tkinter import ttk
import PyUsefulModule


class gui:
    def __init__(self, noter_instance):
        self.window_size = PyUsefulModule.GetScreenResolution()
        self.tk_size = (self.window_size[0]/3, self.window_size[1]-100)
        self.root = tk.Tk(className='Release Noter')
        self.container = ttk.Frame(
            self.root, width=self.tk_size[0], height=self.tk_size[1])
        self.canvas = tk.Canvas(
            self.container, width=self.tk_size[0],  height=self.tk_size[1]-(self.tk_size[1]/10))
        self.scrollbar = ttk.Scrollbar(
            self.container, orient="vertical", command=self.canvas.yview)
        self.scrollable_frame = ttk.Frame(
            self.canvas, width=self.tk_size[0], height=self.tk_size[1])
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(
                scrollregion=self.canvas.bbox("all")
            )
        )
        self.canvas.create_window(
            (0, 0), window=self.scrollable_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=self.scrollbar.set)
        self.commits = []

        self.scrollbar.pack(side='right', fill='y')
        self.container.pack()
        self.canvas.pack(fill='both', expand=True)

        self.canvas.bind_all(
            "<MouseWheel>", lambda event: self.canvas.yview_scroll(-1*int(event.delta/120), "units"))

        self.root.geometry('%dx%d+%d+%d' %
                           (self.tk_size[0], self.tk_size[1], self.window_size[0]/2-self.tk_size[0]/2, self.window_size[1]/2-self.tk_size[1]/2-50))
        self.root.resizable(False, False)

        self.noter_instance = noter_instance

        def showMsgBox(found_dir):
            if found_dir:
                PyUsefulModule.ShowMsgBox(
                    'PyGUI Alert', 'Found git repository in %s' % found_dir)
            else:
                PyUsefulModule.ShowMsgBox(
                    'PyGUI Alert', 'Git repository not found')

        git_found = self.noter_instance.find_git_repository(showMsgBox)

        if not git_found:
            PyUsefulModule.ShowMsgBox(
                'PyGUI Alert', 'Git repository not found')
            exit(-1)

    def set_commit_data(self):
        if not self.noter_instance.list_commit():
            PyUsefulModule.ShowMsgBox(
                'PyGUI Alert', 'List commits failed')
            exit(-1)

        commits = self.noter_instance.get_current_list_commits()

        for commit in commits:
            self.commits.append({
                'title': commit['title'] + ' (%s) ' % commit['author'],
                'is_select': tk.BooleanVar(),
                'component': None
            })

        def left_click_handle(event, cbx, is_select):
            if not is_select.get():
                cbx.select()
            else:
                cbx.deselect()

        for i in range(len(self.commits)):
            commit = self.commits[i]
            note_frame = tk.Frame(self.scrollable_frame, pady=10, padx=10,
                                  highlightbackground='black', highlightthickness=1, height=self.tk_size[1]/10, width=self.tk_size[0]-20)

            note_chk_box = tk.Checkbutton(
                note_frame, variable=commit['is_select'])
            note_chk_box.pack(side='left')
            commit['component'] = note_chk_box

            note_label = tk.Label(
                note_frame, text=commit['title'], justify='left', anchor="w", wraplength=self.tk_size[0]-50)
            note_label.pack(side='left')
            note_label.pack_propagate(0)

            note_label.bind('<Button-1>', lambda event, cbx=note_chk_box,
                            is_select=commit['is_select']: left_click_handle(event, cbx, is_select))

            note_frame.pack()
            note_frame.pack_propagate(0)

            note_frame.bind('<Button-1>', lambda event, cbx=note_chk_box,
                            is_select=commit['is_select']: left_click_handle(event, cbx, is_select))

    def mk_btn(self):
        left_btn = None
        right_btn = None

        def r_btn_callback(event):
            # do sth...
            to_remove = []
            for i in range(len(self.commits)):
                commit = self.commits[i]
                if not commit['is_select'].get():
                    to_remove.append(i)
                commit['component'].config(state=tk.DISABLED)

            self.noter_instance.remove_commit_by_index(to_remove)
            self.noter_instance.copy_release_note_to_clipboard()

            left_btn.config(state=tk.DISABLED)

        def l_btn_callback(event):
            if left_btn['text'] == 'Select All':
                left_btn.config(text='DeSelect All')

                for commit in self.commits:
                    commit['component'].select()

            elif left_btn['text'] == 'DeSelect All':
                left_btn.config(text='Select All')

                for commit in self.commits:
                    commit['component'].deselect()

        btn_frame = tk.Frame(self.root, pady=10, padx=10, height=(
            self.tk_size[1]/10-10), width=self.tk_size[0])

        sub_frame = tk.Frame(btn_frame, pady=10, padx=10, height=(
            self.tk_size[1]/10-10), width=self.tk_size[0])

        left_btn = tk.Button(sub_frame, text='Select All',
                             height=sub_frame.winfo_height(), padx=10, pady=5)
        left_btn.grid()
        left_btn.bind('<Button-1>', l_btn_callback)

        right_btn = tk.Button(sub_frame, text='Copy',
                              height=sub_frame.winfo_height(), padx=10, pady=5)
        right_btn.grid()
        right_btn.bind('<Button-1>', r_btn_callback)

        left_btn.grid(column=0, row=0, padx=10)
        right_btn.grid(column=1, row=0, padx=10)

        sub_frame.pack()
        btn_frame.pack(side='bottom', fill='both')
        btn_frame.pack_propagate(0)

    def run(self):
        self.root.mainloop()
