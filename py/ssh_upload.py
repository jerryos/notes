##!/usr/bin/python
# encoding=utf-8
# Filename: ssh_upload.py
# Paramiko’s documentation: http://docs.paramiko.org/en/1.16/
 
import os
import sys
import time
import paramiko
import logging
 
 
def svnFilter(file_path):
    if '.svn' in file_path: 
        return False
    return True
 
def gitFilter(file_path):
    if '.git' in file_path: 
        return False
    return True
 
def settingFilter(file_path):
    if '.setting' in file_path: 
        return False
    return True
 
class SSHFileUpload:
    """docstring for SSHFileUpload"""
 
    def __init__(self, hostname, port, username, password):
        self.hostname = hostname
        self.port = port
        self.username = username
        self.password = password
        # self._candidate_dirs = []
        self.__ssh = None
        self._transport = None
        self._sftp = None
         
 
    def connect(self):
        self.close()
        self.__ssh_connect()
        self._sftp = self.__ssh.open_sftp()
 
    def reconnect(self):
        self.connect()
        logging.info("SSH session reconnected")
 
    def is_connected(self):
        transport = self.__ssh.get_transport() if self.__ssh else None
        return transport and transport.is_active()
 
    def __ssh_connect(self):
        if (not self.__ssh):
            self.__ssh = paramiko.SSHClient()
			# 遇到新的server，接受新server，将公钥添加到本地的HostKeys
            self.__ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            self.__ssh.connect(self.hostname, self.port, self.username, self.password)
            self._transport = self.__ssh.get_transport()
 
     
    def __ssh_exec(self, command):
        sin, sout, serr = self.__ssh.exec_command(command)
        logging.info("SSH exec_command: %s ", command)
        # if sout.readlines():
        #     logging.info('stdout: %s', sout.readlines())
        # if serr.readlines():
        #     logging.info('stderr: %s', serr.readlines())
            # raise SystemExit
            # sys.exit(1)
        # return sin, sout, serr
         
 
    def exec_command(self, command):
        if not self.is_connected():
            self.reconnect()
        return self.__ssh_exec(command)
 
    def invoke_shell(self, command='', password='abc123!@#'):
        channel = self.__ssh.invoke_shell()
        return channel
         
         
    def __ssh_close(self):
        if (self.__ssh):
            self.__ssh.close()
 
 
    def closeTransport(self):
        if self._transport and self._transport.is_active():
            self._transport.close()
 
 
    def closeSFTP(self):
        if self._sftp:
            self._sftp.close()
 
 
    def close(self):
        self.closeTransport()
        self.closeSFTP()
        self.__ssh_close()
 
 
    def run(self):
        pass
 
 
    def copyFile(self, local_path, remote_path):
        if not self.is_connected():
            self.reconnect()
             
        try:
            self._sftp.put(local_path, remote_path)
        except Exception, e:
            r_dir = os.path.split(remote_path)[0]
            cmd = "mkdir -p %s" % r_dir
            self.__ssh_exec(cmd)
            time.sleep(0.5)
            self._sftp.put(local_path, remote_path)
        logging.info("copy [%s] to remote [%s]", local_path, remote_path)
 
 
    def copyDir(self, local_dir, remote_dir, filters=[gitFilter, svnFilter, settingFilter]):
        logging.info("Coping [%s] to [%s]\n" % (local_dir, remote_dir))
        candidate_dirs = self._walkLocalDirs(local_dir)
        candidate_dirs = self.__doFilters(candidate_dirs, filters)
        local_dir = local_dir.replace('\\', '/')
 
        for i, candidate_path in enumerate(candidate_dirs):
            local_file = candidate_dirs[i]
            remote_file = candidate_dirs[i].replace(local_dir, remote_dir).replace('\\', '/').replace('//', '/')
            # print local_file, " : ", remote_file
            remote_file = unicode(remote_file, 'cp936').encode('utf8')
            self.copyFile(local_file, remote_file)
        logging.info("Copied [%s] to [%s]\n" % (local_dir, remote_dir))
 
    def doUpload(self, local_dir, remote_dir, filters=[]):
        candidate_dirs = self._walkLocalDirs(local_dir)
        candidate_dirs = self.__doFilters(candidate_dirs, filters)
 
        local_dir = os.path.split(local_dir)[0].replace('\\', '/')
 
        for i, candidate_path in enumerate(candidate_dirs):
            local_file = candidate_dirs[i]
            remote_file = candidate_dirs[i].replace(local_dir, remote_dir).replace('\\', '/').replace('//', '/')
             
            try:
                self._sftp.put(local_file, remote_file)
            except Exception, e:
                r_dir = os.path.split(remote_file)[0]
                cmd = "mkdir -p %s" % r_dir
                self.__ssh_exec(cmd)
                time.sleep(0.5)
                self._sftp.put(local_file, remote_file)
                # print e
            logging.info("upload [%s] to remote [%s]", local_file, remote_file)
 
 
    def _walkLocalDirs(self, local_dir): 
        ''' Get all file path in local_dir and RETURN _candidate_dirs list'''
        _candidate_dirs = []
             
        if os.path.isfile(local_dir):
            _candidate_dirs.append(local_dir.replace('\\', '/'))
            return _candidate_dirs
 
        for dir_name, sub_dirs, files in os.walk(local_dir):
            dir_name = dir_name.replace('\\', '/')
            for filename in files:
                _candidate_dirs.append(dir_name + '/' + filename)
        return _candidate_dirs
 
 
    def __doFilters(self, candidate_dirs, filters=[]):
        for func in filters:
            candidate_dirs = filter(func, candidate_dirs)
        return candidate_dirs
 
hostname='your_ip'
username='test'
password='test'
port=22
 
if __name__ == '__main__':
    local_dir = r'E:/workspace/static/images'
    remote_dir = "temp/aaa"
    sfu = SSHFileUpload(hostname, port, username, password)
    # Test
    # sfu.run()
    sfu.connect()
    sfu.copyDir(local_dir, remote_dir)
 
    local_dir = r'E:/workspace/static/images'
    remote_dir = "temp/bbb"
    sfu.doUpload(local_dir, remote_dir, filters=[svnFilter, vipFilter])    
 
    sfu.close()

