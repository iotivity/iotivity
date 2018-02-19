class TestCaseInfo:

    pre_condition = 'pre_condition'
    test_procedure = 'test_procedure'
    post_condition = 'post_condition'
    expected_result = 'expected_result'

    def __init__(self):
        self.defect_type = ''
        self.description = ''
        self.failure_result = ''
        self.failure_message = ''
        self.priority=''
        self.log_path = ''

responsibilty_map = {
    "SM" : ["Jihun Ha", "i.mushfiq", "towhid, i.mushfiq, uzchoi, mg.jeong"],
    "IC" : ["Jee Hyeok Kim", "sarwar.mgm", "towhid, uzchoi, mg.jeong, imtiaz.h, i.mushfiq, sarwar.mgm"],
    "CA" : ["Jaehong Jo", "m.parves", "towhid, m.parves, imtiaz.h, uzchoi, mg.jeong, i.mushfiq"],
    "RE" : ["JungYong KIM", "i.mushfiq", "towhid, i.mushfiq, yeonhunje, uzchoi, mg.jeong"],
    "RC" : ["yeonhunje", "towhid", "towhid, uzchoi, mg.jeong, i.mushfiq"],
    "RI" : ["Ziran Sun", "i.mushfiq", "imtiaz.h, i.mushfiq, uzchoi, habib.virji, rzr, mg.jeong, Jeong, towhid"],
    "ES" : ["Jihun Ha", "sarwar.mgm", "uzchoi, towhid,  sarwar.mgm, mg.jeong, i.mushfiq"],
    "NS" : ["Chihyun Cho", "i.mushfiq", "yeonhunje, uzchoi, mg.jeong, towhid, i.mushfiq"],
    "PM" : ["js126.lee", "adnan.sami", "dongik.lee, towhid,  i.mushfiq, imtiaz.h, uzchoi, adnan.sami"],
    "CS" : ["elestat", "imtiaz.h", "dongik.lee, uzchoi, imtiaz.h, adnan.sami, i.mushfiq, towhid, jminl.choi"],
    "CHP": ["veeraj.sk", "towhid", "ce.abhishek, AshokChanna, kkane, rzr, towhid, uzchoi, i.mushfiq, Veeraj Khokale"]
}

tc_watchers = "i.mushfiq, imtiaz.h, sarwar.mgm, m.parves, adnan.sami"
