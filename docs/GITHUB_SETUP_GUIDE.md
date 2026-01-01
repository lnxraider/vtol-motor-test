# GitHub Setup Guide - VTOL Motor Test System

This guide will walk you through setting up and pushing your project to GitHub.

---

## 📋 Prerequisites

- [ ] Git installed on your computer
- [ ] GitHub account created
- [ ] Project files ready to upload

---

## 🚀 Step-by-Step GitHub Setup

### Step 1: Install Git (if not already installed)

**Windows:**
```bash
# Download from: https://git-scm.com/download/win
# Run installer with default settings
```

**macOS:**
```bash
# Using Homebrew
brew install git

# Or download from: https://git-scm.com/download/mac
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install git
```

**Verify Installation:**
```bash
git --version
# Should show: git version 2.x.x
```

---

### Step 2: Configure Git

```bash
# Set your name (will appear in commits)
git config --global user.name "Your Name"

# Set your email (use your GitHub email)
git config --global user.email "your.email@example.com"

# Verify configuration
git config --list
```

---

### Step 3: Create GitHub Repository

1. **Go to GitHub:** https://github.com
2. **Sign in** to your account
3. **Click** the `+` icon (top right) → **New repository**

**Repository Settings:**
```
Repository name: vtol-motor-test
Description: ESP32-based motor test system with web interface and temperature monitoring
Public/Private: Choose based on preference
☐ Add README.md (we already have one)
☐ Add .gitignore (we already have one)
☐ Add license (we already have MIT license)
```

4. **Click** "Create repository"

---

### Step 4: Organize Your Local Files

**Create project directory structure:**

```bash
# Create main project folder
mkdir vtol-motor-test
cd vtol-motor-test

# Create subdirectories
mkdir docs
mkdir examples
mkdir images
```

**Move files into place:**

```
vtol-motor-test/
├── ESP32_Motor_Test_COMPLETE_v2_5.ino    ← Main sketch
├── README.md                              ← Repository README
├── LICENSE                                ← MIT License
├── .gitignore                             ← Git ignore file
├── CHANGELOG.md                           ← Version history
├── CONTRIBUTING.md                        ← Contribution guide
├── docs/
│   ├── WEB_INTERFACE.md                  ← Web UI docs
│   ├── MAX31855_DIAGNOSIS.md             ← Temp sensor troubleshooting
│   └── HARDWARE_SETUP.md                 ← Wiring guide (create if needed)
├── examples/
│   └── (example sketches if any)
└── images/
    └── (screenshots, photos, diagrams)
```

---

### Step 5: Initialize Local Git Repository

**Navigate to project folder:**
```bash
cd vtol-motor-test
```

**Initialize Git:**
```bash
git init
# Output: Initialized empty Git repository in ...
```

**Add files to staging:**
```bash
# Add all files
git add .

# Or add files individually
git add ESP32_Motor_Test_COMPLETE_v2_5.ino
git add README.md
git add LICENSE
git add .gitignore
git add CHANGELOG.md
git add CONTRIBUTING.md
git add docs/
```

**Check status:**
```bash
git status
# Should show all files ready to commit (green)
```

**Create first commit:**
```bash
git commit -m "Initial commit - ESP32 Motor Test System v2.5.2"
```

---

### Step 6: Connect to GitHub Repository

**Get your repository URL from GitHub:**
- On your repo page, click the green "Code" button
- Copy the HTTPS URL (looks like: https://github.com/YOUR_USERNAME/vtol-motor-test.git)

**Add remote origin:**
```bash
git remote add origin https://github.com/YOUR_USERNAME/vtol-motor-test.git

# Verify remote added
git remote -v
# Should show:
# origin  https://github.com/YOUR_USERNAME/vtol-motor-test.git (fetch)
# origin  https://github.com/YOUR_USERNAME/vtol-motor-test.git (push)
```

---

### Step 7: Push to GitHub

**Push your code:**
```bash
# Push to main branch
git push -u origin main

# If you get an error about 'master' vs 'main':
git branch -M main
git push -u origin main
```

**Enter GitHub credentials when prompted:**
- Username: your GitHub username
- Password: your GitHub personal access token (NOT your regular password)

**Creating a Personal Access Token (if needed):**
1. GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Generate new token (classic)
3. Give it a name: "Git CLI Access"
4. Select scopes: `repo` (full control of private repositories)
5. Generate token
6. **COPY THE TOKEN** (you won't see it again!)
7. Use this token as password when pushing

---

### Step 8: Verify Upload

1. **Go to your GitHub repository page**
2. **Refresh** the page
3. **Verify** all files are uploaded:
   - ✅ README.md displays on repo homepage
   - ✅ All files visible in file browser
   - ✅ Commit message shows correctly

---

## 📁 Adding Files Later

### Add a New File

```bash
# Create or edit file
# example: Add a new document

# Stage the file
git add docs/NEW_DOCUMENT.md

# Commit with descriptive message
git commit -m "Add NEW_DOCUMENT with XYZ information"

# Push to GitHub
git push
```

### Update Existing File

```bash
# Edit the file
# Save changes

# Stage the changes
git add ESP32_Motor_Test_COMPLETE_v2_5.ino

# Commit with descriptive message
git commit -m "Fix DSHOT initialization bug"

# Push to GitHub
git push
```

### Check What Changed

```bash
# See what files are modified
git status

# See detailed changes
git diff ESP32_Motor_Test_COMPLETE_v2_5.ino

# See commit history
git log --oneline
```

---

## 🌿 Working with Branches (Advanced)

### Create Feature Branch

```bash
# Create and switch to new branch
git checkout -b feature/add-sd-logging

# Make changes, then commit
git add .
git commit -m "Add SD card logging feature"

# Push branch to GitHub
git push -u origin feature/add-sd-logging
```

### Merge Feature to Main

```bash
# Switch back to main
git checkout main

# Merge feature branch
git merge feature/add-sd-logging

# Push updated main
git push

# Delete feature branch (optional)
git branch -d feature/add-sd-logging
```

---

## 📝 Commit Message Best Practices

**Format:**
```
<type>: <short description>

<optional longer description>
```

**Types:**
- `feat:` New feature
- `fix:` Bug fix
- `docs:` Documentation changes
- `style:` Formatting, no code change
- `refactor:` Code restructuring
- `test:` Adding tests
- `chore:` Maintenance tasks

**Examples:**
```bash
git commit -m "feat: Add PWM/DSHOT mode selector to web interface"

git commit -m "fix: Correct voltage calculation formula (division not multiplication)"

git commit -m "docs: Update README with library installation instructions"

git commit -m "refactor: Simplify sensor reading function"
```

---

## 🔄 Keeping Repository Updated

### Pull Latest Changes

```bash
# Fetch and merge changes from GitHub
git pull

# Or fetch without merging
git fetch
```

### Handle Merge Conflicts

If you get merge conflicts:

```bash
# 1. Git will mark conflict files
# 2. Open conflicted files
# 3. Look for conflict markers:
#    <<<<<<< HEAD
#    Your changes
#    =======
#    Other changes
#    >>>>>>> branch-name

# 4. Manually resolve conflicts
# 5. Remove conflict markers
# 6. Stage resolved files
git add <resolved-file>

# 7. Complete merge
git commit -m "Merge: Resolve conflicts in <file>"

# 8. Push
git push
```

---

## 📊 Adding Images/Diagrams

### Recommended Structure

```
images/
├── web_interface.png          # Screenshot of web UI
├── hardware_setup.jpg         # Photo of wiring
├── wiring_diagram.png         # Schematic diagram
└── temperature_graph.png      # Example graph
```

### Add Images

```bash
# Add images to folder
cp ~/Downloads/screenshot.png images/web_interface.png

# Stage and commit
git add images/
git commit -m "docs: Add web interface screenshot"
git push
```

### Reference in README

```markdown
![Web Interface](images/web_interface.png)

or

<img src="images/web_interface.png" width="600" alt="Web Interface">
```

---

## 🏷️ Creating Releases

### Create a Release on GitHub

1. **Go to** your repository page
2. **Click** "Releases" (right sidebar)
3. **Click** "Create a new release"

**Release Settings:**
```
Tag version: v2.5.2
Release title: Version 2.5.2 - Web Interface with Mode Selector
Description:
  - Added PWM/DSHOT mode selector
  - Added 5% and 10% throttle buttons
  - Fixed DSHOT continuous update
  - Bug fixes and improvements
  
Attach binaries: (optional - compiled .bin file)
☐ This is a pre-release
```

4. **Click** "Publish release"

---

## 🛠️ Troubleshooting

### "Permission denied (publickey)"

**Solution:** Use HTTPS instead of SSH, or set up SSH keys:
```bash
# Switch to HTTPS
git remote set-url origin https://github.com/YOUR_USERNAME/vtol-motor-test.git
```

### "Updates were rejected"

**Solution:** Pull first, then push:
```bash
git pull origin main
# Resolve any conflicts
git push origin main
```

### "fatal: not a git repository"

**Solution:** Initialize git in correct directory:
```bash
# Make sure you're in project folder
cd vtol-motor-test
git init
```

### Large files (>100MB)

**Solution:** Use .gitignore to exclude:
```bash
# Add to .gitignore
*.bin
*.hex
large_data_file.csv

# Remove from git if already added
git rm --cached large_file.bin
git commit -m "Remove large file from repo"
```

---

## ✅ Quick Reference Cheat Sheet

```bash
# Setup (one time)
git init
git remote add origin https://github.com/YOUR_USERNAME/vtol-motor-test.git

# Daily workflow
git status                    # Check what changed
git add .                     # Stage all changes
git commit -m "message"       # Commit with message
git push                      # Push to GitHub

# Update from GitHub
git pull                      # Get latest changes

# Check history
git log --oneline            # See commit history
git diff                     # See uncommitted changes

# Branches
git branch feature/new       # Create branch
git checkout feature/new     # Switch to branch
git merge feature/new        # Merge branch into current
```

---

## 📞 Getting Help

**Git Documentation:** https://git-scm.com/doc  
**GitHub Guides:** https://guides.github.com  
**Common Issues:** https://docs.github.com/en/github/getting-started-with-github

**Check Git Status:**
```bash
git status    # Always run this first when confused!
```

---

## 🎉 Success Checklist

After completing these steps, you should have:

- [x] Git installed and configured
- [x] GitHub repository created
- [x] Local repository initialized
- [x] Files organized in proper structure
- [x] Initial commit made
- [x] Code pushed to GitHub
- [x] Repository visible on GitHub
- [x] README displays correctly
- [x] All files uploaded successfully

---

**Congratulations! Your VTOL Motor Test System is now on GitHub!** 🚀

You can now:
- Share your repository link with others
- Accept contributions
- Track changes over time
- Create releases
- Collaborate with team members

Repository URL: `https://github.com/YOUR_USERNAME/vtol-motor-test`
